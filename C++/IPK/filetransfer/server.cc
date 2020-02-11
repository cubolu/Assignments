/******************************************************************************/
/* server.cc                                                                  */
/* Name:        IPK - Project 2, File Transfer Server                         */
/* Compiled:    gcc version 4.9.3 (Ubuntu 14.04)                              */
/* Author:      Jakub Lukac                                                   */
/* e-mail:      xlukac09@stud.fit.vutbr.cz                                    */
/* Created:     22-04-2016                                                    */
/******************************************************************************/

#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <csignal>
#include <string>
#include <cstring>
#include <cerrno>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>  // Types used in sys/socket.h and netinet/in.h
#include <sys/socket.h> // Core socket functions and data structures
#include <netinet/in.h> // Internet domain address structures and functions
#include <arpa/inet.h>  // Functions for manipulating numeric IP addresses
#include <netdb.h>      // Used for domain/DNS hostname lookup
#include <unistd.h>     // Close socket function

#define ARGUMENTS   "p:"    // define input arguments
#define QUEUE_LIMIT 256
#define BUFFER_SIZE 256
enum class Mode {DOWNLOAD_FROM_SERVER, UPLOAD_TO_SERVER};

#define ERROR_OPEN_FILE     1
#define ERROR_CREATE_FILE   2
#define ERROR_INTERNAL      3
#define ERROR_UNKNOWN       100

int GLOBAL_SOCKFD;

void signal_handler(int signum) {

#ifndef NDEBUG
    std::cout << "info: Closing connection after SIGINT..." << std::endl;
#endif

    close(GLOBAL_SOCKFD);
    // wait for children
    while (waitpid(-1, NULL, 0)) {
       if (errno == ECHILD) {
          break;
       }
    }
    exit(signum);

}

void send_ok(int sockfd) {                                              // child

    std::stringstream ss;
    ss << "OK " << 0 << "\n";

    ssize_t n = send(sockfd, ss.str().c_str(), ss.str().length(), 0);
    if (n == -1) {
        std::cerr << "error: " << std::strerror(errno) << std::endl;
        close(sockfd);
        exit(EXIT_FAILURE);
    }

}

void send_error(int sockfd, int code, std::string msg) {                // child
    // send error response and exit
    std::stringstream ss;
    ss << "ERROR " << code << " " << msg << "\n";

    ssize_t n = send(sockfd, ss.str().c_str(), ss.str().length(), 0);
    if (n == -1) {
        std::cerr << "error: " << std::strerror(errno) << std::endl;
    }
    close(sockfd);
    exit(EXIT_FAILURE);

}

std::string send_response(int sockfd, const Mode m, const std::string filename, std::fstream& f) {  // child

    std::string tmpname;

    switch (m) {
        case Mode::DOWNLOAD_FROM_SERVER:
            f.open(filename, std::fstream::in);
            if (!f.is_open()) {
                std::cerr << "error: " << std::strerror(errno) << std::endl;
                send_error(sockfd, ERROR_OPEN_FILE, "Cannot open file");
            }
            break;
        case Mode::UPLOAD_TO_SERVER:
            // open temporary file
            char temp[] = "tmpfileXXXXXX";
            tmpname = mktemp(temp);
            if (tmpname.empty()) {
                std::cerr << "error: " << std::strerror(errno) << std::endl;
                send_error(sockfd, ERROR_INTERNAL, "Internal server error");
            }
            f.open(tmpname, std::fstream::out);
            if (!f.is_open()) {
                std::cerr << "error: " << std::strerror(errno) << std::endl;
                send_error(sockfd, ERROR_CREATE_FILE, "Cannot create file");
            }
            break;
    }

    send_ok(sockfd);
    return tmpname;

}

void send_file(int sockfd, std::fstream& f) {                           // child

    char buff[BUFFER_SIZE];
    while (!f.eof()) {
        f.read(buff, BUFFER_SIZE);
        ssize_t n = send(sockfd, buff, f.gcount(), 0);
        //std::cout << "S: " << n << ", " << f.gcount() << std::endl;
        //std::cout << "B: " << std::string(buff, n) << std::endl;
        if (n == -1) {
            std::cerr << "error: " << std::strerror(errno) << std::endl;
            f.close();
            close(sockfd);
            exit(EXIT_FAILURE);
        }
    }

}

void recv_file(int sockfd, std::fstream& f) {                           // child

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
        f.close();
        close(sockfd);
        exit(EXIT_FAILURE);
    }

}

std::string get_part(int sockfd) {                                      // child

    std::string s = "";
    char c;
    ssize_t n;
    do {
        n = recv(sockfd, &c, 1, MSG_WAITALL);
        s += c;
    } while (n > 0 && c != ' ');

    if (n == -1) {
        std::cerr << "error: " << std::strerror(errno) << std::endl;
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    s.pop_back();   // pop delimiter
    return s;

}

void recv_request(int sockfd, Mode& m, std::string& filename) {         // child

    std::string mode = get_part(sockfd);
    if (mode == "DOWNLOAD")
        m = Mode::DOWNLOAD_FROM_SERVER;
    else if (mode == "UPLOAD")
        m = Mode::UPLOAD_TO_SERVER;
    else {
        std::cerr << "error: " << "Corrupted protocol" << std::endl;
        send_error(sockfd, ERROR_UNKNOWN, "Corrupted protocol");
    }

    size_t name_len;
    try {
        name_len = std::stoi(get_part(sockfd));
    }
    catch ( ... ) {
        std::cerr << "error: " << "Cannot convert string to integer" << std::endl;
        send_error(sockfd, ERROR_UNKNOWN, "Corrupted protocol");
    }

    std::vector<char> buff(name_len + 1);
    ssize_t n;
    n = recv(sockfd, &buff[0], name_len+1, MSG_WAITALL);

    if (n == -1) {
        std::cerr << "error: " << std::strerror(errno) << std::endl;
        send_error(sockfd, ERROR_INTERNAL, "Internal server error");
    } else if (n != name_len+1){
        std::cerr << "error: " << "Corrupted protocol" << std::endl;
        send_error(sockfd, ERROR_UNKNOWN, "Corrupted protocol");
    }

    buff[name_len] = '\0';
    filename = std::string(buff.begin(), buff.end());

}

int main(int argc, char* argv[]) {

    int port = 0;

    int opt;
    while ((opt = getopt(argc, argv, ARGUMENTS)) != -1) {
        switch (opt) {
            case 'p':
                try {
                    port = std::stoi(optarg);
                }
                catch ( ... ) {
                    std::cerr << "error: " << "Incorrect port option" << std::endl;
                    std::cerr << "Usage: " << argv[0] << " -p PORT" << std::endl;
                    return EXIT_FAILURE;
                }
                break;
            case '?':
            default:
                std::cerr << "Usage: " << argv[0] << " -p PORT" << std::endl;
                return EXIT_FAILURE;
                break;
        }   // end switch
    }   // end while
    if (argc < 2 || optind != argc) {
        std::cerr << "error: " << "Incorrect options" << std::endl;
        std::cerr << "Usage: " << argv[0] << " -p PORT" << std::endl;
        return EXIT_FAILURE;
    }

    int welcome_socket;
    int connection;
    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // create socket
    if ((welcome_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        std::cerr << "error: Cannot create socket: " << std::strerror(errno) << std::endl;
        return EXIT_FAILURE;
    }

    // set global socket descriptor and register signal SIGINT
    GLOBAL_SOCKFD = welcome_socket;
    if (std::signal(SIGINT, signal_handler) == SIG_ERR) {
        std::cerr << "error: Cannot set a signal handler: " << std::strerror(errno) << std::endl;
        close(welcome_socket);
        return EXIT_FAILURE;
    }

    // allow bind() to reuse local addresses
    int one = 1;
    setsockopt(welcome_socket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));

    // initialize server address struct
    memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // assign a socket to a local address
    if (bind(welcome_socket, reinterpret_cast<struct sockaddr *>(&serv_addr), sizeof(serv_addr)) == -1) {
        std::cerr << "error: Bind failed: " << std::strerror(errno) << std::endl;
        close(welcome_socket);
        return EXIT_FAILURE;
    }

    // enable willingness to accept incoming connections and set a queue limit
    if (listen(welcome_socket, QUEUE_LIMIT) == -1) {
        std::cerr << "error: Listen failed: " << std::strerror(errno) << std::endl;
        close(welcome_socket);
        return EXIT_FAILURE;
    }

    while (true) {
        // accept incoming connection request
        if ((connection = accept(welcome_socket, reinterpret_cast<struct sockaddr *>(&client_addr), &client_addr_len)) < 0) {
            std::cerr << "error: Accept failed: " << std::strerror(errno) << std::endl;
            break;
        }

        // communication
        pid_t process_id = fork();
        if (process_id == 0) {          // child
            // communicate with client
            close(welcome_socket);      // close in child process

            Mode m;
            std::string file_name;
            recv_request(connection, m, file_name);

#ifndef NDEBUG
            std::string m_str;
            if (m == Mode::DOWNLOAD_FROM_SERVER)
                m_str = "DOWNLOAD";
            else
                m_str = "UPLOAD";
            std::cout << "info: Mode - " << m_str << ", file - " << file_name << std::endl;
#endif

            std::fstream f;
            std::string tmp_name = send_response(connection, m, file_name, f);

            if (m == Mode::DOWNLOAD_FROM_SERVER) {
                send_file(connection, f);
                f.close();
            } else {    // UPLOAD_TO_SERVER
                recv_file(connection, f);
                f.close();
                if(std::rename(tmp_name.c_str(), file_name.c_str())) {
                    std::cerr << "error: Rename failed: " << std::strerror(errno) << std::endl;
                    close(connection);
                    exit(EXIT_FAILURE);
                }
            }

#ifndef NDEBUG
            std::cout << "info: Closing connection..." << std::endl;
#endif

            // correctly shut down connection
            if (shutdown(connection, SHUT_RDWR) == -1) {
              std::cerr << "error: Shutdown failed: " << std::strerror(errno) << std::endl;
              close(connection);
              exit(EXIT_FAILURE);
            }

            close(connection);
            exit(EXIT_SUCCESS);
        } else if (process_id > 0) {    // parent
            // welcome next client

#ifndef NDEBUG
            char cl_addt_str[INET_ADDRSTRLEN];
            if(inet_ntop(AF_INET, &client_addr.sin_addr, cl_addt_str, sizeof(cl_addt_str))) {
                std::cout << "Client connected" << std::endl;
                std::cout << "info: Client address: " << cl_addt_str;
                std::cout << ":" << ntohs(client_addr.sin_port) << std::endl;
            }
#endif

            close(connection);          // close in parent process
        } else {
            std::cerr << "error: Fork failed: " << std::strerror(errno) << std::endl;
            close(connection);
            break;
        }
    }

    close(welcome_socket);

    // wait for children
    while (waitpid(-1, NULL, 0)) {
       if (errno == ECHILD) {
          break;
       }
    }

    return EXIT_FAILURE;

}
