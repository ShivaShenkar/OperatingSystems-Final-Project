#pragma once
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <limits>
#include <algorithm>

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
bool is_number(const string& s) {
    return !s.empty() && all_of(s.begin(), s.end(), ::isdigit);
}
int main(){
    int sockfd = setup_client();
    string fullMsg,msgV,msgE,msgS;
    int e,v,s;
    bool flagS;
    //infinte loop to send commands to server, command "exit" will close the connection
    
    
    while(true){
        cout<<"Type in the amount of vertices and edges you want to send to the server (exit to quit)\n"<<endl;
        while(true){
            cout << "Type in the amount of vertices: ";

            getline(cin, msgV);
            if(msgV == "exit") {
                close(sockfd);
                return 0;
            }
            try{
                v = stoi(msgV);
                if(v>0)
                    break;
                else{
                    throw invalid_argument("not positive");
                }
            }
            catch(...){
                cout<<"Invalid input. Please enter a positive integer."<<endl;
                cin.clear();
            }

        }
        cin.clear();
        while(true){
            cout << "Type in the amount of edges: ";
            getline(cin, msgE);
            if(msgE == "exit") {
                close(sockfd);
                return 0;
            }
            try{
                e = stoi(msgE);
                if( e>0&&e<=v*(v-1)/2)
                    break;
                else{
                    throw invalid_argument("not positive");
                }
            }
            catch(...){
                cout<<"Invalid input. Please enter a positive integer."<<endl;
                cin.clear();
            }

        }
        cin.clear();
        while(true){
            cout << "Would you like to add seed manually? (y/n): ";
            
            getline(cin, msgS);
            if(msgS == "exit") {
                close(sockfd);
                return 0;
            }
            string lowercase_answer = msgS; 

            transform(lowercase_answer.begin(), lowercase_answer.end(), lowercase_answer.begin(),
                   [](unsigned char c){ return std::tolower(c); });
                
            if(lowercase_answer == "y"){
                flagS = true;
                break;
            }
            else if(lowercase_answer == "n"){
                flagS = false;
                break;
            }
            else{
                cout<<"Invalid input. Please enter 'y' or 'n'."<<endl;
                cin.clear();
            }

        }
        if(flagS){
            cout << "Type in the seed: ";
            cin.clear();
            getline(cin, msgS);
            if(msgS == "exit") {
                close(sockfd);
                return 0;
            }
        }
        fullMsg = to_string(v) + " " + to_string(e) + (flagS ? " "+msgS : "");
        send(sockfd, fullMsg.c_str(), fullMsg.size(), 0);
        char buf[1024];
        int n = read(sockfd, buf, sizeof(buf));
        if (n > 0) {
            cout << "Server response: " << string(buf, n) << endl;
        }
        else {
            cout << "Server closed the connection." << endl;
            break;
        }
    }
    close(sockfd);
    return 0;
}