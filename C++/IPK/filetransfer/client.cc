/******************************************************************************/
/* client.cc                                                                  */
/* Name:        IPK - Project 2, File Transfer Client                         */
/* Compiled:    gcc version 4.9.3 (Ubuntu 14.04)                              */
/* Author:      Jakub Lukac                                                   */
/* e-mail:      xlukac09@stud.fit.vutbr.cz                                    */
/* Created:     22-04-2016                                                    */
/******************************************************************************/

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <sys/types.h>  // Types used in sys/socket.h and netinet/in.h
#include <sys/socket.h> // Core socket functions and data structures
#include <netinet/in.h> // Internet domain address structures and functions
#include <arpa/inet.h>  // Functions for manipulating numeric IP addresses
#include <netdb.h>      // Used for domain/DNS hostname lookup
#include <unistd.h>     // Close socket function

#define ARGUMENTS   "h:p:d:u:"    // define input arguments
#define BUFFER_SIZE 256
enum class Mode {DOWNLOAD, UPLOAD};

void send_request(int sockfd, Mode m, std::string file_name) {

    std::stringstream ss;
    switch (m) {
        case Mode::DOWNLOAD:
            ss << "DOWNLOAD " << file_name.length() << " " << file_name << "\n";
            break;
        case Mode::UPLOAD:
            ss << "UPLOAD " << file_name.length() << " " << file_name << "\n";
            break;
    }

    ssize_t n = send(sockfd, ss.str().c_str(), ss.str().length(), 0);
    if (n == -1) {
        std::cerr << "error: " << std::strerror(errno) << std::endl;
        close(sockfd);
        exit(EXIT_FAILURE);
    }

}

std::string get_part(int sockfd, char delim) {

    std::string s = "";
    char c;
    ssize_t n;
    do {
        n = recv(sockfd, &c, 1, MSG_WAITALL);
        s += c;
    } while (n > 0 && c != delim);

    if (n == -1) {
        std::cerr << "error: " << std::strerror(errno) << std::endl;
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    s.pop_back();   // pop delimiter
    return s;

}

void handle_response(int sockfd) {

    std::string status = get_part(sockfd, ' ');
    if (status == "OK") {
        ssize_t n;
        char buff[2];
        n = recv(sockfd, buff, 2, MSG_WAITALL);
        if (n == -1) {
            std::cerr << "error: " << std::strerror(errno) << std::endl;
        } else if (n == 2 && buff[0] == '0' && buff[1] == '\n') {
            // communication successfuly initialize
            return;
        } else {
            std::cerr << "error: " << "Corrupted protocol" << std::endl;
        }
    } else if (status == "ERROR") {
        size_t code;
        try {
            code = std::stoi(get_part(sockfd, ' '));
            std::cerr << "error: server: " << code << " - " << get_part(sockfd, '\n') << std::endl;
        }
        catch ( ... ) {
            std::cerr << "error: " << "Corrupted protocol" << std::endl;
        }
    } else {
        std::cerr << "error: " << "Corrupted protocol" << std::endl;
    }
    close(sockfd);
    exit(EXIT_FAILURE);

}

void recv_file(int sockfd, std::fstream& f) {

    char buff[BUFFER_SIZE];
    ssize_t n;
    do {
        n = recv(sockfd, buff, BUFFER_SIZE, MSG_WAITALL);
        f.write(buff, n);
        //std::cout << "R: " << n << std::endl;
        //std::cout << "B: " << std::string(buff, n) << std::endl;
    } while (n > 0);
    if (n == -1) {
        std::cerr << "error: " << std::strerror(errno) << std::endl;
        close(sockfd);
        exit(EXIT_FAILURE);
    }

}

void send_file(int sockfd, std::fstream& f) {

    char buff[BUFFER_SIZE];
    while (!f.eof()) {
        f.read(buff, BUFFER_SIZE);
        ssize_t n = send(sockfd, buff, f.gcount(), 0);
        //std::cout << "S: " << n << ", " << f.gcount() << std::endl;
        //std::cout << "B: " << std::string(buff, n) << std::endl;
        if (n == -1) {
            std::cerr << "error: " << std::strerror(errno) << std::endl;
            close(sockfd);
            exit(EXIT_FAILURE);
        }
    }

}

int main(int argc, char* argv[]) {

    char * host = nullptr;
    uint16_t port = 0;
    std::string file_name;
    bool download = false;
    bool upload = false;

    int opt;
    while ((opt = getopt(argc, argv, ARGUMENTS)) != -1) {
        switch (opt) {
            case 'h':
                host = optarg;
                break;
            case 'p':
                try {
                    port = std::stoi(optarg);
                }
                catch ( ... ) {
                    std::cerr << "error: " << "Incorrect port option" << std::endl;
                    std::cerr << "Usage: " << argv[0] << " -h HOST -p PORT [-d|-u] FILE" << std::endl;
                    return EXIT_FAILURE;
                }
                break;
            case 'd':
                download = true;
                file_name = optarg;
                break;
            case 'u':
                upload = true;
                file_name = optarg;
                break;
            case '?':
            default:
                std::cerr << "Usage: " << argv[0] << " -h HOST -p PORT [-d|-u] FILE" << std::endl;
                return EXIT_FAILURE;
                break;
        }   // end switch
    }   // end while
    if (argc < 2 || optind != argc || host == nullptr || port == 0 || file_name.empty()) {
        std::cerr << "error: Incorrect options" << std::endl;
        std::cerr << "Usage: " << argv[0] << " -h HOST -p PORT [-d|-u] FILE" << std::endl;
        return EXIT_FAILURE;
    }

    Mode m;
    std::fstream f;
    if (download && upload) {
        std::cerr << "error: 'd' and 'u' are mutually exclusive options" << std::endl;
        std::cerr << "Usage: " << argv[0] << " -h HOST -p PORT [-d|-u] FILE" << std::endl;
        return EXIT_FAILURE;
    } else if (download) {
        m = Mode::DOWNLOAD;
        f.open(file_name, std::fstream::out);
    } else {
        m = Mode::UPLOAD;
        f.open(file_name, std::fstream::in);
    }

    if (!f.is_open()) {
        std::cerr << "error: Cannot open/create file" << std::endl;
        return EXIT_FAILURE;
    }

#ifndef NDEBUG
    std::cout << "Host:  " << host << std::endl;
    std::cout << "Port:  " << port << std::endl;
    std::cout << (download ? "Download: " : "Upload: ") << file_name << std::endl;
#endif

    int socket_handle;
    struct hostent *server;
    struct sockaddr_in serv_addr;

    // get IP address
    if ((server = gethostbyname(host)) == nullptr) {
        if (errno == 0)
            std::cerr << "error: No such host as \"" << host << "\"" << std::endl;
        else
            std::cerr << "error: " << std::strerror(errno) << std::endl;
        return EXIT_FAILURE;
    }

    // create socket
    if ((socket_handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        std::cerr << "error: Cannot create socket: " << std::strerror(errno) << std::endl;
        return EXIT_FAILURE;
    }

    // initialize server address struct
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(port);
    memcpy(&serv_addr.sin_addr, server->h_addr, server->h_length);

    // establish a connection
    if (connect(socket_handle, reinterpret_cast<struct sockaddr *>(&serv_addr), sizeof(serv_addr))) {
        std::cerr << "error: " << std::strerror(errno) << std::endl;
        close(socket_handle);
        return EXIT_FAILURE;
    }

    send_request(socket_handle, m, file_name);
    handle_response(socket_handle);

    if (m == Mode::DOWNLOAD) {
        recv_file(socket_handle, f);
    } else {    // UPLOAD
        send_file(socket_handle, f);
    }

    close(socket_handle);
    f.close();

    return EXIT_SUCCESS;

}
