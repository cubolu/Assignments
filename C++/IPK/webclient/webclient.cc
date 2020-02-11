/******************************************************************************/
/* webclient.cc                                                               */
/* Name:        IPK - Project 1, Web Client                                   */
/* Compiled:    gcc version 4.9.3 (Ubuntu 14.04)                              */
/* Author:      Jakub Lukac                                                   */
/* e-mail:      xlukac09@stud.fit.vutbr.cz                                    */
/* Created:     01-04-2016                                                    */
/******************************************************************************/

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstring>
#include <regex>
#include <cerrno>
#include <sys/types.h>  // Types used in sys/socket.h and netinet/in.h
#include <sys/socket.h> // Core socket functions and data structures
#include <netinet/in.h> // Internet domain address structures and functions
#include <arpa/inet.h>  // Functions for manipulating numeric IP addresses
#include <netdb.h>      // Used for domain/DNS hostname lookup
#include <unistd.h>     // Close socket function

#define PORT            80
#define VERSION_1_0     '0'
#define VERSION_1_1     '1'

void send_request(int sockfd, std::string host, std::string& res, char ver) {

    std::stringstream ss;
    ss << "GET " << res << " HTTP/1." << ver << "\r\n";
    ss << "Host: " << host << "\r\n";
    ss << "\r\n" << std::flush;

    ssize_t n = send(sockfd, ss.str().c_str(), ss.str().length(), 0);
    if (n == -1) {
        std::cerr << "ERROR: " << std::strerror(errno) << std::endl;
        close(sockfd);
        exit(EXIT_FAILURE);
    }

}

std::string get_line(int sockfd) {

    std::string line  =  "";
    char c = '\0';
    char prev_c = '\0';
    ssize_t n;
    do {
        n = recv(sockfd, &c, 1, MSG_WAITALL);
        line += c;
        if (prev_c == '\r' && c == '\n')
            break;  // end of line
        prev_c = c;
    } while (n > 0);

    if (n == -1) {
        std::cerr << "ERROR: " << std::strerror(errno) << std::endl;
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    return line;

}

bool is_chunked(std::string str) {

    std::smatch str_matches;
    std::regex e ("Transfer-Encoding: (.*)\r\n", std::regex::icase);
    std::regex_search(str, str_matches, e);

#ifndef NDEBUG
    // debug info
    std::cerr << "content length: ";
    for (unsigned i = 0; i < str_matches.size(); ++i) {
        std::cerr << "[" << str_matches[i] << "] ";
    }
    std::cerr << std::endl;
#endif

    if (str_matches.size() != 2) {
        return false;
    }

    return str_matches[1] == "chunked";

}

size_t content_len(std::string str) {

    std::smatch str_matches;
    std::regex e ("Content-Length: (\\d*)\r\n", std::regex::icase);
    std::regex_search(str, str_matches, e);

#ifndef NDEBUG
    // debug info
    std::cerr << "content length: ";
    for (unsigned i = 0; i < str_matches.size(); ++i) {
        std::cerr << "[" << str_matches[i] << "] ";
    }
    std::cerr << std::endl;
#endif

    if (str_matches.size() != 2) {
        return 0;
    }

    return std::stol(str_matches[1]);

}

void recv_file(int sockfd, size_t msg_len, std::ofstream& file) {

    char c;
    ssize_t n;
    for (size_t i = 0; i < msg_len; ++i) {
        n = recv(sockfd, &c, 1, MSG_WAITALL);
        if (n < 0) {
            std::cerr << "ERROR: " << std::strerror(errno) << std::endl;
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        // write content to file
        file << c;
    }

}

void recv_chunks(int sockfd, std::ofstream& file) {

    std::string ch_size_str; // chunk size
    size_t ch_size;

    do {
        ch_size_str = get_line(sockfd);
        ch_size = std::stol(ch_size_str, 0, 16);
        // write chunk to file
        recv_file(sockfd, ch_size, file);
        get_line(sockfd); // read empty line
    } while(ch_size != 0);

}

void recv_data(int sockfd, std::string header, std::string fname) {

    std::ofstream f (fname);
    if (!f.is_open()) {
        std::cerr << "ERROR: " << "Unable to open file" << std::endl;
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (is_chunked(header)) {
        recv_chunks(sockfd, f);
    } else {
        recv_file(sockfd, content_len(header), f);
    }

    f.close();
}

std::string new_location(std::string str) {

    std::smatch str_matches;
    std::regex e ("Location: (http://.*)\r\n", std::regex::icase);
    std::regex_search(str, str_matches, e);

#ifndef NDEBUG
    // debug info
    std::cerr << "content length: ";
    for (unsigned i = 0; i < str_matches.size(); ++i) {
        std::cerr << "[" << str_matches[i] << "] ";
    }
    std::cerr << std::endl;
#endif

    if (str_matches.size() != 2) {
        std::cerr << "ERROR: " << "Invalid location" << std::endl;
        exit(EXIT_FAILURE);
    }

    return str_matches[1];

}

std::string handle_response(int sockfd, std::string file_name) {

    std::stringstream ss;

    // read status line
    ss << get_line(sockfd);

    std::smatch str_matches;
    std::regex e ("^HTTP/\\d+\\.\\d+ (\\d+) (.+)\r\n");
    std::regex_match(ss.str(), str_matches, e);

#ifndef NDEBUG
    // debug info
    std::cerr << "the matches were: ";
    for (unsigned i = 1; i < str_matches.size(); ++i) {
        std::cerr << "[" << str_matches[i] << "] ";
    }
    std::cerr << std::endl;
#endif

    if (str_matches.size() != 3) {
        std::cerr << "ERROR: " << "Invalid status line" << std::endl;
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    int status_code = std::stoi(str_matches[1]);
    std::string msg = str_matches[2];

    // read header
    std::string act_line;
    do {
        act_line = get_line(sockfd);
        ss << act_line;
    } while (act_line != "\r\n");

#ifndef NDEBUG
    // debug info
    std::cerr << ss.str();
#endif

    switch (status_code) {
        case 200:   // OK
            recv_data(sockfd, ss.str(), file_name);
            close(sockfd);
            exit(EXIT_SUCCESS);
            break;
        case 301:   // Moved Permanently
        case 302:   // Found
            return new_location(ss.str());
            break;
        case 505:   // HTTP Version not supported
            return "505";
            break;
        default:
            if (status_code < 400)
                std::cerr << "ERROR: " << "Status-Code" << str_matches[1]
                          << " not supported - " << msg
                          << std::endl;
            else
                std::cerr << "ERROR: " << msg << std::endl;
            close(sockfd);
            exit(EXIT_FAILURE);
            break;
    }

}

int main(int argc, char* argv[]) {

    // check arguments count
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " url" << std::endl;
        return EXIT_FAILURE;
    }

    std::string url (argv[1]);
    std::string domain;
    std::string resource;
    std::string file_name;
    bool file_name_set = false;

    int socket_handle;
    uint16_t default_port = PORT;
    struct hostent *server;
    struct sockaddr_in serv_addr;


    std::smatch str_matches;
    std::regex e_url ("http://([\\w\\d](?:[\\w\\d\\-\\.]*[\\w\\d]|))(?:\\:(\\d+)|)(?:(/[^?]*)(?:\\?.*|)|)");
    // std::regex e_fname ("/([^/]*)$");

    for (int i = 0; i < 5; ++i) {

        std::regex_match(url, str_matches, e_url);

#ifndef NDEBUG
        // debug info
        std::cerr << "the matches were: ";
        for (unsigned i = 1; i < str_matches.size(); ++i) {
            std::cerr << "[" << str_matches[i] << "] ";
        }
        std::cerr << std::endl;
#endif

        if (str_matches.size() != 4) {
            std::cerr << "ERROR: " << "Invalid URL" << std::endl;
            return EXIT_FAILURE;
        }

        // set domain and resource, also check value of port
        domain.assign(str_matches[1]);
        if (!((std::string)str_matches[2]).empty() && std::stoi(str_matches[2]) != 80) {
            std::cerr << "ERROR: " << "Invalid URL port - " << str_matches[2] << std::endl;
            return EXIT_FAILURE;
        }
        if (!((std::string)str_matches[3]).empty()) {
            resource.assign(str_matches[3]);
        } else {
            resource.assign("/");
        }

        // set file name
        if (!file_name_set) {
            std::size_t start = resource.rfind('/');
            std::string new_file_name = resource.substr(start + 1);
            if (!new_file_name.empty()) {
                file_name.assign(new_file_name);
            } else {
                file_name.assign("index.html");
            }
            file_name_set = true;
        }

#ifndef NDEBUG
        // debug info
        std::cerr << "File      \"" << file_name    << "\"" << std::endl;
        std::cerr << "Domain    \"" << domain       << "\"" << std::endl;
        std::cerr << "Resource  \"" << resource     << "\"" << std::endl;
#endif

        // get IP address
        if ((server = gethostbyname(domain.c_str())) == NULL) {
            if (errno == 0)
                std::cerr << "ERROR: No such host as \"" << domain << "\"" << std::endl;
            else
                std::cerr << "ERROR: " << std::strerror(errno) << std::endl;
            close(socket_handle);
            return EXIT_FAILURE;
        }

        // create socket
        if ((socket_handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
            std::cerr << "ERROR: Cannot create socket" << std::endl;
            std::cerr << "\t" << std::strerror(errno) << std::endl;
            return EXIT_FAILURE;
        }

        // initialize server address struct
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(default_port);
        memcpy(&serv_addr.sin_addr, server->h_addr, server->h_length);

        // establish a connection
        if (connect(socket_handle, reinterpret_cast<struct sockaddr *>(&serv_addr), sizeof(serv_addr))) {
            std::cerr << "ERROR: " << std::strerror(errno) << std::endl;
            close(socket_handle);
            return EXIT_FAILURE;
        }

        send_request(socket_handle, domain, resource, VERSION_1_1);

        // return only if the requested resource resides under a different URL
        url = handle_response(socket_handle, file_name);

        if (url == "505") {
            send_request(socket_handle, domain, resource, VERSION_1_0);
            url = handle_response(socket_handle, file_name);
        }

        close(socket_handle);

    } // end for

    std::cerr << "ERROR: The maximum number of redirections was reached" << std::endl;

    return EXIT_FAILURE;

}
