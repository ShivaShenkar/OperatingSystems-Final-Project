#include "../part-2/euler.cpp"
#include <getopt.h>
#include <cstring>
#include <random>
#include <set>
#include <chrono>

Graph generateRandomGraph(int v,int e,int s=random()){
    mt19937 rng;
    rng.seed(s);
    cout<<"Seed: "<<s<<endl;
    uniform_int_distribution<int> dist(0,v-1);

    vector<set<int>> currEdges;
    Graph g(v);
    while(currEdges.size() < e){
        bool flag=false;
        int u = dist(rng);
        int w = dist(rng);
        if(u != w){
            for(auto &edge : currEdges){
                if(edge.find(u)!=edge.end() &&edge.find(w)!=edge.end()){
                    flag=true;
                    break;
                }
            }
            if(!flag){
                currEdges.push_back({u,w});
                g.addEdge(u,w,0);
            }
        }
    }

    cout<<"Generated Random Graph with "<<v<<" vertices and "<<e<<" edges"<<endl;
    cout<<"Edges:"<<endl;
    for(auto &edge : currEdges){
        for(auto &v : edge){
            cout<<v<<" ";
        }
        cout<<endl;
    }
    
    return g;
}