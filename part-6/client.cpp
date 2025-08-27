#pragma once
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>

using namespace std;


int setup_client(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        cerr << "Error creating socket" << endl;
        exit(EXIT_FAILURE);
    }
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(9034);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if(connect(sockfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
        cerr << "Error connecting to server" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "Connected to server on port 9034" << endl;
    return sockfd;
}
int main(){
    int sockfd = setup_client();
    string command;
    //infinte loop to send commands to server, command "exit" will close the connection
    
    cout<<" Type in the amount of vertices and edges you want to send to the server (exit to quit)"<<endl;
    while(true){
        cout << "Type in the amount of vertices: ";
        getline(cin, command);
        if(command == "exit") {
            close(sockfd);
            return 0;
        }
        do{

        }
        while(command!=)
        send(sockfd, command.c_str(), command.size(), 0);
        char buf[1024];
        int n = read(sockfd, buf, sizeof(buf));
        if (n > 0) {
            cout << "Server response: " << string(buf, n) << endl;
        }
    }
    close(sockfd);
    return 0;
}