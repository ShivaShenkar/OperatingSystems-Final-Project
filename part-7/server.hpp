#ifndef SERVER_HPP
#define SERVER_HPP
#include "algos.hpp"     
#include "randomGraph.hpp" 
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <string>
#include <vector>
#include <poll.h>

class Server {
public:
    explicit Server(int port = 9034);
    ~Server();

    void run();

private:
    int sockfd;
    std::vector<pollfd> fds;

    int setup_socket(int port);
    std::string process_message(const std::string& msg);

    void handle_new_connection();
    void handle_client_message(int index);
};

#endif // SERVER_HPP
