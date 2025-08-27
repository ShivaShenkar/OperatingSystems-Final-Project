#pragma once
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <sstream>
#include <vector>
#include <poll.h>
using namespace std;


int setup_socket(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        cerr << "Error creating socket" << endl;
        exit(EXIT_FAILURE);
    }
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(9034);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if(bind(sockfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress))){
        perror("bind");
        cerr << "Error binding socket" << endl;
        exit(EXIT_FAILURE);
    }

    if(listen(sockfd, SOMAXCONN)<0){
        cerr << "Error listening on socket" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "Server listening on port 9034" << endl;
    return sockfd;
}


int main(){
    int sockfd = setup_socket();

    vector<pollfd> fds;
    fds.push_back({sockfd,POLLIN,0});

    while(true){
        int ready = poll(&fds[0], fds.size(), -1);
        if(ready < 0) {
            std::cerr << "Error in poll" << std::endl;
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        if (fds[0].revents & POLLIN){
            //client-connection events
            int client_fd = accept(sockfd, nullptr, nullptr);
            if (client_fd >= 0) {
                std::cout<< "New client connected." <<std::endl;
                fds.push_back({client_fd, POLLIN});
            }
        }
        for(int i=1;i<fds.size();i++){
            if(fds[i].revents & POLLIN){
                //program goes here when a client sends a message or disconnects
                char buffer[1024] = {0};
                int n = read(fds[i].fd, buffer, sizeof(buffer));
                if(n<=0){
                    close(fds[i].fd);
                    fds.erase(fds.begin() + i);
                    i--;
                    std::cout << "Client disconnected." <<std::endl;
                }
                else{
                    buffer[n]='\0';
                    std::cout << "Message from client: " << buffer << std::endl;
                    string bufStr(buffer);
                    
                }
            }
        }
        
    }
    close(sockfd);
    return 0;
}