/**
 * myldap.cpp
 * Project:     ISA Assignment, LDAP Server
 * Compiled:    gcc version 4.9.3 (Ubuntu 16.04)
 * Author:      Jakub Lukac
 * e-mail:      xlukac09@stud.fit.vutbr.cz
 * Created:     08-10-2017
 *
 */

#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <csignal>
#include <cstring>
#include <sys/wait.h>

#include <sys/types.h>  // Types used in sys/socket.h and netinet/in.h
#include <sys/socket.h> // Core socket functions and data structures
#include <netinet/in.h> // Internet domain address structures and functions
#include <arpa/inet.h>  // Functions for manipulating numeric IP addresses
#include <netdb.h>      // Used for domain/DNS hostname lookup
#include <unistd.h>     // Close socket function

#include "ldap.h"

#define QUEUE_LIMIT 256
#define DEFAULT_PORT 389

#define ARGUMENTS   "p:f:"    // define input arguments
#define PRINT_USAGE(name) (std::cerr << "Usage: " << (name) << " [-p port] -f file" << std::endl)

int GLOBAL_SOCKFD;

void signal_handler(int signum) {

    std::cout << "info: Closing connection after SIGINT..." << std::endl;
    close(GLOBAL_SOCKFD);
    // wait for children
    while (waitpid(-1, nullptr, 0)) {
        if (errno == ECHILD) {
            break;
        }
    }
    std::exit(signum);

}

void main_loop(int welcome_socket, std::string& file_name) {
    int connection;
    struct sockaddr_in client_addr{};
    socklen_t client_addr_len = sizeof(client_addr);

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

            try {
                // process client request
                Ldap ldap_protocol(connection, file_name);
                while (ldap_protocol.is_clientActive())
                    ldap_protocol.process_request();
            } catch (const std::exception &e) {
                 std::cerr << e.what() << std::endl;
            }

#ifndef NDEBUG
            std::cout << "info: Closing connection..." << std::endl;
#endif

            std::exit(EXIT_SUCCESS);
        } else if (process_id > 0) {    // parent
            // welcome next client

#ifndef NDEBUG
            // client statistics
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
}

int main(int argc, char* argv[]) {
    int port_arg = DEFAULT_PORT;
    std::string file_name;

    // parse arguments
    int opt;
    while ((opt = getopt(argc, argv, ARGUMENTS)) != -1) {
        switch (opt) {
            case 'p':
                try {
                    port_arg = std::stoi(optarg);
                }
                catch ( ... ) {
                    std::cerr << "Incorrect port option." << std::endl;
                    PRINT_USAGE(argv[0]);
                    return EXIT_FAILURE;
                }
                break;
            case 'f':
                file_name = optarg;
                break;
            case '?':
            default:
                PRINT_USAGE(argv[0]);
                return EXIT_FAILURE;
        }   // end switch
    }   // end while
    // check file name and possible extra arguments
    if (file_name.empty() || optind != argc) {
        std::cerr << "Incorrect options." << std::endl;
        PRINT_USAGE(argv[0]);
        return EXIT_FAILURE;
    }

    // verify that port number is valid port
    if (port_arg <= 0 || port_arg >= 65536) {
        std::cerr << "Invalid port value '" << port_arg << "'." << std::endl;
        PRINT_USAGE(argv[0]);
        return EXIT_FAILURE;
    }
    auto port = static_cast<uint16_t>(port_arg);
    // verify that file name is a file and can be opened
    if (!std::ifstream(file_name).good()) {
        std::cerr << "Invalid file, error occurred while opening file: '" << file_name << "'." << std::endl;
        PRINT_USAGE(argv[0]);
        return EXIT_FAILURE;
    }

    int welcome_socket = 0;
    struct sockaddr_in serv_addr{};

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

    main_loop(welcome_socket, file_name);

    close(welcome_socket);

    // wait for children
    while (waitpid(-1, nullptr, 0)) {
        if (errno == ECHILD) {
            break;
        }
    }

    return EXIT_FAILURE;
}

