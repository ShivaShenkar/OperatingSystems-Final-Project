#pragma once
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <sstream>
#include <vector>
#include <poll.h>
#include "/home/yuvaliloo/Desktop/OSPROJECT/OperatingSystems-Final-Project/part-4/randomGraph.cpp"
using namespace std;


int setup_socket(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        cerr << "Error creating socket" << endl;
        exit(EXIT_FAILURE);
    }
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(sockfd);
        return 1;
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

string proccess_message(const string& msg){
    stringstream ss1(msg);
    stringstream ss2(msg);
    int n1,n2;
    string seed;
    string res = ""; 
    if(ss1>>n1>>n2){
        Graph g = generateRandomGraph(n1,n2);
        res = findEulerianCircle(g);
        return res;
    }
    else if(ss1>>n1>>n2>>seed){
        try{
            int s = stoi(seed);
            Graph g = generateRandomGraph(n1,n2,s);
            res = findEulerianCircle(g);
            return res;
        }
        catch(...){
            int seedNum = 0; 
            size_t len = strlen(seed.c_str());
            size_t copy_len = min(len, sizeof(seedNum));
            memcpy(&seedNum, seed.c_str(), copy_len);
            Graph g = generateRandomGraph(n1,n2,seedNum);
            res = findEulerianCircle(g);
            return res;
        }
    }
    return "Invalid input format. Please provide <vertices> <edges> [seed]";

}


int main(){
    int sockfd = setup_socket();

    vector<pollfd> fds;
    fds.push_back({sockfd,POLLIN,0});

    while(true){
        int ready = poll(&fds[0], fds.size(), -1);
        if(ready < 0) {
            cerr << "Error in poll" << endl;
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        if (fds[0].revents & POLLIN){
            //client-connection events
            int client_fd = accept(sockfd, nullptr, nullptr);
            if (client_fd >= 0) {
                cout<< "New client connected." <<endl;
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
                    cout << "Client disconnected." <<endl;
                }
                else{
                    buffer[n]='\0';
                    cout << "Message from client: " << buffer << endl;
                    string bufStr(buffer);
                    string response = proccess_message(bufStr);
                    cout<<"Sending response to client.\n"<<endl;
                    send(fds[i].fd, response.c_str(), response.size(), 0);
                }
            }
        }
        
    }
    close(sockfd);
    return 0;
}