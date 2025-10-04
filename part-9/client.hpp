#pragma once
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <algorithm>

class Client {
private:
    int sockfd;

    int setup_client() {
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0) {
            std::cerr << "Error creating socket" << std::endl;
            exit(EXIT_FAILURE);
        }

        sockaddr_in serverAddress{};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(9034); // make sure matches server
        serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

        if (connect(fd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
            std::cerr << "Error connecting to server" << std::endl;
            exit(EXIT_FAILURE);
        }

        std::cout << "Connected to server on port 9034" << std::endl;
        return fd;
    }

    bool is_number(const std::string& s) {
        return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
    }

public:
    Client() {
        sockfd = setup_client();
    }

    ~Client() {
        close(sockfd);
    }

    void run() {
        std::string algo, msgV, msgE, msgS, fullMsg;
        int v, e;
        bool useSeed;

        while (true) {

            // Vertices
            while (true) {
                std::cout << "Number of vertices: ";
                std::getline(std::cin, msgV);
                if (msgV == "exit") return;
                try { v = std::stoi(msgV); if (v > 0) break; }
                catch(...) { }
                std::cout << "Please enter a positive integer.\n";
            }

            // Edges
            while (true) {
                std::cout << "Number of edges: ";
                std::getline(std::cin, msgE);
                if (msgE == "exit") return;
                try { e = std::stoi(msgE); if (e >= 0 && e <= v*(v-1)/2) break; }
                catch(...) { }
                std::cout << "Enter a valid number of edges (0 <= edges <= vertices*(vertices-1)/2).\n";
            }

            
            fullMsg =  std::to_string(v) + " " + std::to_string(e);
            

            // Send request to server
            send(sockfd, fullMsg.c_str(), fullMsg.size(), 0);

            // Receive response
            char buf[4096] = {0};
            int n = read(sockfd, buf, sizeof(buf));
            if (n > 0) {
                std::cout << "\nServer response:\n" << std::string(buf, n) << "\n" << std::endl;
            } else {
                std::cout << "Server closed the connection." << std::endl;
                break;
            }
        }
    }
};
