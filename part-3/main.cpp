#include "../part-2/euler.cpp"
#include <getopt.h>
#include <cstring>
#include <random>
#include <set>
#include <chrono>

int main(int argc , char* argv[]){
    int option_index = 0;
    int c;
    bool flagV=false,flagE=false,flagS=false;
    int edges,vertices;
    size_t seed;

    static struct option long_options[] = {
        {"edges",    required_argument,0,'e'},
        {"vertices",  required_argument, 0,'v'},
        {"seed", required_argument,0,'s'},
        {0,0,0,0}
    };
    opterr = 0; // Disable automatic error messages
    while ((c = getopt_long(argc, argv, "e:v:s:", long_options, &option_index)) != -1) {
        switch(c){
            case 'e':
                try{
                    edges = stoi(optarg);
                    flagE =true;
                }
                catch(...){
                    cerr<<"Invalid argument for edges"<<endl;
                    return 1;
                }
                break;

            case 'v':
                try{
                    vertices = stoi(optarg);
                    flagV =true;
                }
                catch(...){
                    cerr<<"Invalid argument for vertices"<<endl;
                    return 1;
                }
                break;
            case 's':
                if(optarg){
                    flagS=true;
                    seed = 0; // reset
                    size_t len = strlen(optarg);
                    size_t copy_len = min(len, sizeof(seed));
                    memcpy(&seed, optarg, copy_len);
                }
                break;
            case '?':
                if(optopt=='e'){
                    cerr<<"Option --edges requires an argument."<<endl;
                    return 1;
                }
                else if(optopt=='v'){
                    cerr<<"Option --vertices requires an argument."<<endl;
                    return 1;
                }
                break;
        }
    }
    if(!flagE || !flagV){
        cerr<<"Missing required arguments"<<endl;
        return 1;
    }
    if(vertices<0){
        cerr<<"Invalid amount of vertices"<<endl;
        return 1;
    }
    if(edges<0||edges>(vertices*(vertices-1))/2){
        cerr<<"Invalid amount of edges"<<endl;
        return 1;
    }
    mt19937 rng;
    if(!flagS){
        seed=std::chrono::high_resolution_clock::now().time_since_epoch().count();
    }
    rng.seed(seed);
    cout<<"Seed: "<<seed<<endl;
    uniform_int_distribution<int> dist(0,vertices-1);

    vector<set<int>> currEdges;
    Graph g(vertices);
    while(currEdges.size() < edges){
        bool flag=false;
        int u = dist(rng);
        int v = dist(rng);
        if(u != v){
            for(auto &edge : currEdges){
                if(edge.find(u)!=edge.end() &&edge.find(v)!=edge.end()){
                    flag=true;
                    break;
                }
            }
            if(!flag){
                currEdges.push_back({u,v});
                g.addEdge(u,v,0);
            }
        }
    }

    cout<<"Generated Random Graph with "<<vertices<<" vertices and "<<edges<<" edges."<<endl;
    cout<<"Edges:"<<endl;
    for(auto &edge : currEdges){
        for(auto &v : edge){
            cout<<v<<" ";
        }
        cout<<endl;
    }
    findEulerianCircle(g);
    return 0;
}