

void addRandomWeightsDirectedGraph(Graph& g){
    cout<<"Adding random weights to the directed graph..."<<endl;
    int n = g.getVertices();
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            if(g.isConnected(i,j)){
                double weight = random() %201;
                g.setWeight(i,j,weight);
                cout<<"Edge: "<<i<<"->"<<j<<" Weight: "<<weight<<endl;
            }
        }
    }
}
void addRandomWeightsUndirectedGraph(Graph& g){
    int n = g.getVertices();
    for(int i=0;i<n;i++){
        for(int j=i+1;j<n;j++){
            if(g.isConnected(i,j)){
                double weight = random() %201;
                g.setWeight(i,j,weight);
                cout<<"Edge: {"<<i<<","<<j<<"} Weight: "<<weight<<endl;
            }
        }
    }
}

Graph generateRandomDirectedGraph(int v,int e,int s=random()){
    mt19937 rng;
    rng.seed(s);
    cout<<"Seed: "<<s<<endl;
    uniform_int_distribution<int> dist(0,v-1);

    vector<pair<int,int>> currEdges;
    Graph g(v);
    while(currEdges.size() < e){
        bool flag=false;
        int u = dist(rng);
        int w = dist(rng);
        if(u != w){
            for(auto &edge : currEdges){
                if(edge.first == u && edge.second == w){
                    flag=true;
                    break;
                }
            }
            if(!flag){
                currEdges.push_back({u,w});
                g.addDirectedEdge(u,w,0);
            }
        }
    }

    cout<<"Generated Random Directed Graph with "<<v<<" vertices and "<<e<<" edges"<<endl;
    cout<<"Edges:"<<endl;
    for(auto &edge : currEdges){
        cout<<edge.first<<" -> "<<edge.second;
        cout<<endl;
    }
    
    return g;
}


Graph buildUndirectedGraph(){
    int v=setVertices();
    int e = setEdges(v);
    bool random= RandomGraphSelect();
    
    if(random){
        Graph g =generateRandomGraph(v,e);
        return g;
    }
    Graph g(v);
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
        currEdges.push_back({v1,v2});
        g.addEdge(v1,v2,0);
    }
    return g;
}


Graph buildDirectedWeightedGraph(){
    int v=setVertices();
    int e = setEdges(v);
    bool random= RandomGraphSelect();

    if(random){
        Graph g =generateRandomDirectedGraph(v,e);
        addRandomWeightsDirectedGraph(g);
        return g;
    }
    Graph g(v);
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
        auto it = find_if(currEdges.begin(),currEdges.end(),[v1,v2](const pair<int,int>& edge){
            return edge.first==v1 && edge.second==v2;
        });
        if(it!=currEdges.end()){
            cout<<"Invalid input. Edge already exists."<<endl;
            continue;
        }

        cout<<"Enter the weight of the edge:"<<endl;
        double weight = numberInputDouble();
        currEdges.push_back({v1,v2});
        g.addDirectedEdge(v1,v2,weight);
    
    }
    return g;
}

    



Graph buildUndirectedWeightedGraph(){
    int v=setVertices();
    int e = setEdges(v);
    bool random= RandomGraphSelect();

    if(random){
        Graph g =generateRandomGraph(v,e);
        addRandomWeightsUndirectedGraph(g);
        return g;
    }
    Graph g(v);
    vector<set<int>> currEdges;
    while(currEdges.size()<e){
        cout<<"Enter your source vertex of edge number "<<currEdges.size()+1<<endl;
        int v1 = numberInput(0,v-1);
        cout<<"Enter your target vertex of edge number "<<currEdges.size()+1<<endl;
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
        
        cout<<"Enter the weight of the edge:"<<endl;
        double weight = numberInputDouble();
        currEdges.push_back({v1,v2});
        g.addEdge(v1,v2,weight);
    }
    return g;
}