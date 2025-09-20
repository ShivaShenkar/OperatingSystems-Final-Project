#pragma once
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <utility>

using namespace std;


int numberInput(int min=numeric_limits<int>::min(),int max=numeric_limits<int>::max()){
    
    while(true){
        string inputStr;
        getline(cin,inputStr);
        try{
            int input = stoi(inputStr);
            if(input<min||input>max)
                throw invalid_argument("Invalid input");
            return input;
        }
        catch(...){
            bool b1 = (min==numeric_limits<int>::min());
            bool b2 = (max==numeric_limits<int>::max());
            if(b1 && b2){
                cout<<"Invalid input. Please enter an integer."<<endl;
            }
            else if(b1){
                cout<<"Invalid input. Please enter an integer less than or equal to "<<max<<endl;
            }
            else if(b2){
                cout<<"Invalid input. Please enter an integer greater than or equal to "<<min<<endl;
            }
            else{
                cout<<"Invalid input. Please enter an integer between "<<min<<" and "<<max<<endl;
            }
            cin.clear();
        }
    }
}
double numberInputDouble(double min=numeric_limits<double>::min(),double max=numeric_limits<double>::max()){
    while(true){
        string inputStr;
        getline(cin,inputStr);
        try{
            double input = stod(inputStr);
            if(input<min||input>max)
                throw invalid_argument("Invalid input");
            return input;
        }
        catch(...){
            bool b1 = (min==numeric_limits<double>::min());
            bool b2 = (max==numeric_limits<double>::max());
            if(b1 && b2){
                cout<<"Invalid input. Please enter a number."<<endl;
            }
            else if(b1){
                cout<<"Invalid input. Please enter a number less than or equal to "<<max<<endl;
            }
            else if(b2){
                cout<<"Invalid input. Please enter a number greater than or equal to "<<min<<endl;
            }
            else{
                cout<<"Invalid input. Please enter a number between "<<min<<" and "<<max<<endl;
            }
            cin.clear();
        }
    }
}

bool yesNoInput(){
    bool res;
    while(true){
        string response;
        getline(cin, response);
        string lowercase_answer = response;
        
        transform(lowercase_answer.begin(), lowercase_answer.end(), lowercase_answer.begin(),
                   [](unsigned char c){ return tolower(c); });
       
        if(lowercase_answer == "y"){
            res= true;
            break;
        }
        else if(lowercase_answer == "n"){
            res= false;
            break;
        }
        else{
            cout<<"Invalid input. Please enter 'y' or 'n'."<<endl;
        }
    }
    return res;
}

int setVertices(){
    cout<<"Enter number of vertices:"<<endl;
    return numberInput(1);
}

int setEdges(int vertices){
    cout<<"Enter number of edges: [0 -"<<vertices*(vertices-1)/2<<"]"<<endl;
    return numberInput(0,vertices*(vertices-1)/2);
}

bool RandomGraphSelect(){
    cout<<"Would like to set a random graph? (y/n)"<<endl;
    bool res = yesNoInput();
    return res;
}

string setup_edges(int v,int e,string& g){
    string res="";
    if(g == "U"||g=="UW"){
        vector<set<int>> currEdges;
        while(currEdges.size()<e){
            cout<<"Enter your first vertex of edge number "<<currEdges.size()+1<<endl;
            int v1 = numberInput(0,v-1);
            cout<<"Enter your second vertex of edge number "<<currEdges.size()+1<<endl;
            int v2 = numberInput(0,v-1);
            if(v1==v2){
                cout<<"Invalid input. Please enter two different vertices."<<endl;
                continue;
            }
            auto it = find_if(currEdges.begin(),currEdges.end(),[v1,v2](const set<int>& edge){
                return edge.find(v1)!=edge.end() && edge.find(v2)!=edge.end();
            });
            if(it!=currEdges.end()){
                cout<<"Invalid input. Edge already exists."<<endl;
                continue;
            }
            
            if(g=="UW"){
                cout<<"Enter the weight of the edge:"<<endl;
                double weight = numberInputDouble();
                res = res+ "\n" + to_string(v1) + " " + to_string(v2) + " " + to_string(weight);

            }
            else{
                res = res+ "\n" + to_string(v1) + " " + to_string(v2);
            }
            currEdges.push_back({v1,v2});
        }
    }
    else{
        vector<pair<int,int>> currEdges;
        while(currEdges.size()<e){
            cout<<"Enter your source vertex of edge number "<<currEdges.size()+1<<endl;
            int v1 = numberInput(0,v-1);
            cout<<"Enter your target vertex of edge number "<<currEdges.size()+1<<endl;
            int v2 = numberInput(0,v-1);
            if(v1==v2){
                cout<<"Invalid input. Please enter two different vertices."<<endl;
                continue;
            }
            for(auto& edge: currEdges){
                if(edge.first==v1 && edge.second==v2){
                    cout<<"Invalid input. Edge alrady exists."<<endl;
                    continue;
                }
            }
            cout<<"Enter the weight of the edge:"<<endl;
            double weight = numberInputDouble();
            res = res+ "\n" + to_string(v1) + " " + to_string(v2) + " " + to_string(weight);
            currEdges.push_back({v1,v2});
        }
    }
    return res;
}


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
    //infinte loop to send commands to server, command "exit" will close the connection
    while(true){
        int select=-1;
        string gType="";
        cout<<"Select your desired algorithm"<<endl;
        cout<<"0:kruskal 1:SCC 2:edomonds karp 3:max clique"<<endl;
        select = numberInput(0,3);
        
        switch (select)
        {
            case 0:
                {
                cout<<"You selected Kruskal algorithm"<<endl;
                cout<<"Building undirected weighted graph..."<<endl;
                gType="UW";
                
                }
                break;   
            case 1:
                {
                cout<<"You selected SCC algorithm"<<endl;
                cout<<"Building undirected graph..."<<endl;
                gType="U";
                
                }
                break;
            case 2:
                {
                cout<<"You selected Edmonds-Karp algorithm"<<endl;
                cout<<"Building directed weighted graph..."<<endl;
                gType="DW";
                
                }
                break;
            case 3:
                {
                cout<<"You selected max-clique algorithm"<<endl;
                cout<<"Building undirected graph..."<<endl;
                gType="U";
                
                }
                break;
            default:
                cout<<"Error, closing programm..."<<endl;
                close(sockfd);
                return 0;
                break;
        }
        int v=setVertices();
        int e = setEdges(v);
        bool random= RandomGraphSelect();
        char r = random ? 'R' : 'M';
        string msg = gType +" "+ r + " " + to_string(v) + " " + to_string(e);
        if(!random){
            msg = msg + setup_edges(v,e,gType);
        }
        send(sockfd, msg.c_str(),msg.size(),0);
        char buf[1024];
        int n = read(sockfd, buf, sizeof(buf));
        if (n > 0) {
            cout << "Server response: " << string(buf, n) << endl;
        }
        else {
            cout << "Server closed the connection." << endl;
            break;
        }

        cout<<"Would you like to run another algorithm? (y/n)"<<endl;
        bool cont = yesNoInput();
        if(!cont)
            break;
    }
    cout<<"Closing connection..."<<endl;
    close(sockfd);
    return 0;
}

