#include "server.hpp"
#include "algos.hpp"
#include "randomGraph.hpp"
#include <memory>
#include <thread>
#include <sstream>

using namespace std;

Server::Server(int port) {
    sockfd = setup_socket(port);
    fds.push_back({sockfd, POLLIN, 0});
}

Server::~Server() {
    close(sockfd);
}

int Server::setup_socket(int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        cerr << "Error creating socket" << endl;
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(fd);
        exit(EXIT_FAILURE);
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("bind");
        cerr << "Error binding socket" << endl;
        exit(EXIT_FAILURE);
    }

    if (listen(fd, SOMAXCONN) < 0) {
        cerr << "Error listening on socket" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Server listening on port " << port << endl;
    return fd;
}

void Server::run() {
    while (true) {
        int ready = poll(&fds[0], fds.size(), -1);
        if (ready < 0) {
            cerr << "Error in poll" << endl;
            exit(EXIT_FAILURE);
        }

        if (fds[0].revents & POLLIN) {
            handle_new_connection();
        }

        for (int i = 1; i < fds.size(); i++) {
            if (fds[i].revents & POLLIN) {
                handle_client_message(i);
            }
        }
    }
}

void Server::handle_new_connection() {
    int client_fd = accept(sockfd, nullptr, nullptr);
    if (client_fd >= 0) {
        cout << "New client connected." << endl;
        fds.push_back({client_fd, POLLIN, 0});
    }
}

void Server::handle_client_message(int index) {
    char buffer[1024] = {0};
    int n = read(fds[index].fd, buffer, sizeof(buffer));
    if (n <= 0) {
        close(fds[index].fd);
        fds.erase(fds.begin() + index);
        cout << "Client disconnected." << endl;
    } else {
        buffer[n] = '\0';
        string msg(buffer);

        // Spawn a detached worker thread
        int client_fd = fds[index].fd;
        thread([this, msg, client_fd]() {
            string response = process_message(msg);
            send(client_fd, response.c_str(), response.size(), 0);
            cout << "Processed request for client " << client_fd << endl;
        }).detach();
    }
}

std::string Server::process_message(const std::string& msg) {
    stringstream ss(msg);
    string algo;
    int vertices, edges;
    ss >> algo >> vertices >> edges;
    if (algo.empty()) return "Invalid request format.\n";

    if (vertices <= 0 || edges < 0) return "Error: invalid number of vertices or edges.\n";

    Graph g = generateRandomGraph(vertices, edges);

    try {
        unique_ptr<AlgoStrategy> s(AlgoFactory::createAlgo(algo, g));
        return s->execute();  // using factory + polymorphism
    } catch (const exception& ex) {
        return string("Error: ") + ex.what() + "\n";
    }
}
