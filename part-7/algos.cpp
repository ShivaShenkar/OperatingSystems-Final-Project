#include "union_find.cpp"
#include "../part-2/euler.cpp"
#include <algorithm>
#include <limits>
#include <queue>


typedef pair<int,int> twoPoints;

//////////////////////////ALGO NUMBER 3//////////////////////////
vector<pair<twoPoints,double>> kruskal(const Graph& g){
    vector <pair<twoPoints,double>> mst;
    int n=g.getVertices();
    UnionFind uf(n);
    vector<tuple<double,int,int>> edges;
    for(int i=0;i<n;++i){
        for(int j=i+1;j<n;++j){
            if(g.isConnected(i,j)){
                edges.push_back({g.getWeight(i,j),i,j});
            }
        }
    }
    sort(edges.begin(), edges.end());
    for(auto& edge:edges){
        int u = get<1>(edge);
        int v = get<2>(edge);
        if(uf.find(u) != uf.find(v)){
            uf.unite(u,v);
            mst.push_back({{u,v},get<0>(edge)});
        }
    }
    return mst;
}


//////////////////////////ALGO NUMBER 5//////////////////////////
vector<vector<int>> get_SCC(const Graph& g){
    vector<int> currVertices(g.getVertices());
    vector<vector<int>> scc;
    iota(currVertices.begin(), currVertices.end(), 0);
    vector<bool> visited(g.getVertices(),false);
    while(!currVertices.empty()){
        dfs(g,currVertices[0],visited);
        vector<int> comp;
        for(auto& v: currVertices){
            if(visited[v]){
                comp.push_back(v);
                currVertices.erase(remove(currVertices.begin(), currVertices.end(), v), currVertices.end());
            }
        }
        if(!comp.empty()){
            scc.push_back(comp);
        }
    }

    return scc;
}


//////////////////////////ALGO NUMBER 6//////////////////////////
bool isPathConnected(const Graph& g, int i,int j){
    vector<bool> visited(g.getVertices(),false);
    dfs(g,i,visited);
    return visited[j];
}

struct FlowEdge{
    double capacity;
    double flow;
    double residualCapacity() const {
        return capacity - flow;
    }
};
class FlowNetwork{
    private:
        bool** edges;
        FlowEdge** f_edges;
        int n;
    public:
        
        FlowNetwork(const Graph& g) : n(g.getVertices()){
            edges = new bool*[n];
            f_edges = new FlowEdge*[n];

            for(int i=0;i<n;++i){
                edges[i] = new bool[n];
                f_edges[i] = new FlowEdge[n];
            }
            for(int i=0;i<n;++i){
                for(int j=0;j<n;++j){
                    edges[i][j] = g.isConnected(i,j);
                    if(edges[i][j]){
                        f_edges[i][j].capacity = g.getWeight(i,j);
                        f_edges[i][j].flow = 0;
                    }
                }  
            }
            
        }
        ~FlowNetwork(){
            for(int i=0;i<n;++i){
                delete[] edges[i];
                delete[] f_edges[i];
            }
            delete[] edges;
            delete[] f_edges;
        }
        bool isConnected(int i,int j) const{
            if(i!=j&&i>=0&&j>=0&&i<n&&j<n){
                return edges[i][j]; 
            }
            return false;
        }
        int getVertices() const {return n;}
        double getresidualCapacity(int i,int j) const{
            if(isConnected(i,j))
                return f_edges[i][j].residualCapacity();
            throw ("invalid vertices\no edge between the two vertices");
        }
        double getFlow(int i,int j) const{
            if(isConnected(i,j))
                return f_edges[i][j].flow;
            throw ("invalid vertices\no edge between the two vertices");
        }
        double getCapacity(int i,int j) const{
            if(isConnected(i,j))
                return f_edges[i][j].capacity;
            throw ("invalid vertices\no edge between the two vertices");
        }
        void addFlow(int i,int j, double amount){
            if(isConnected(i,j)){
                if(f_edges[i][j].residualCapacity()>=amount){
                    f_edges[i][j].flow += amount;
                }
                else
                    throw ("not enough residual capacity");
            }
            else
                throw ("invalid vertices\no edge between the two vertices");
        }

};


vector<pair<twoPoints,int>> findAugmentingPath(const FlowNetwork& fn){
    int n = fn.getVertices();
    vector<bool> visited = vector<bool>(n,false);
    vector<int> parentType = vector<int>(n,0); // true for forward edge, false for reverse edge
    vector<int> parent = vector<int>(n,-1);
    queue<int> q;
    bool reached = false;
    visited[0] = true;
    q.push(0);
    while (!q.empty()&&!reached) {
        int u = q.front(); q.pop();
        bool foundNext = false;
        for(int i=0;i<n;++i){
            bool b1 = fn.isConnected(u,i),b2 = fn.isConnected(i,u);
            int isForward; //0 means reverse edge, 1 means forward edge, 2 means both
            double maxResCap; // the maximal residual capacity of the edge (u,i)
            if((b1||b2) && !visited[i]){
                if(b1&&b2){
                    maxResCap = fn.getresidualCapacity(u,i)+fn.getFlow(i,u);
                    isForward = 2;
                }
                else if(b1){
                    maxResCap = fn.getresidualCapacity(u,i);
                    isForward = 1;
                }
                else{
                    maxResCap = fn.getFlow(i,u);
                    isForward = 0;
                }
                if(maxResCap>0 && !visited[i]){
                    foundNext = true;
                    parent[i] = u;
                    visited[i] = true;
                    parentType[i] = isForward;
                    q.push(i);
                    if(i==n-1){
                        reached = true;
                        break;
                    }
                }
            }
        }
        if(!foundNext)
            return {};
    }
    vector<pair<twoPoints,int>> res;
    int curr = n-1;
    while(curr!=0){
        twoPoints p(curr,parent[curr]);
        res.emplace_back(p,parentType[curr]);
        curr = parent[curr];
    }
    return res;
}   


FlowNetwork edmonds_carp(const Graph& g){
    FlowNetwork fn(g);
    if(!isPathConnected(g,0,g.getVertices()-1)){
        cout<<"No path from source to sink"<<endl;
        return fn;
    }
    while(true){
        vector<pair<twoPoints,int>> augPath = findAugmentingPath(fn);
        if(augPath.empty())
            break;
        double minResCap = numeric_limits<double>::max(); // minimum residual capacity of the augmenting path
        for(auto& [edge,isForward]: augPath){

            if(isForward) //if edge is forward
                minResCap = min(minResCap,fn.getresidualCapacity(edge.second,edge.first));
            else // edge is reverse
                minResCap = min(minResCap,fn.getFlow(edge.first,edge.second));

        }

        for(auto& [edge,isForward]: augPath){
            if(isForward) //if edge is forward
                fn.addFlow(edge.second,edge.first,minResCap);
            else // edge is reverse
                fn.addFlow(edge.first,edge.second,-minResCap);
        }

    }
    return fn;

}



//////////////////////////ALGO NUMBER 1//////////////////////////
vector<int> max_clique(const Graph& g){
    int n = g.getVertices();
    vector<int> maxClique;
    for(int i=(1<<n)-1;i>=1;--i){
        vector<int> currClique;
        bool isClique = true;
        for(int j=0;j<n;++j){
            if(i&(1<<j)){
                for(int k=0;k<currClique.size();++k){
                    if(!g.isConnected(j,currClique[k])){
                        isClique = false;
                        break;
                    }
                }
                if(!isClique)
                    break;
                currClique.push_back(j);
            }
        }
        if(isClique && currClique.size()>maxClique.size()){
            maxClique = currClique;
        }
    }
    return maxClique;
}


class AlgoStrategy{
    protected:
        Graph& g;
        AlgoStrategy(Graph& graph) : g(graph) {}
    public:
        virtual ~AlgoStrategy() = default;
        virtual void execute() = 0;
};

class KruskalAlgo : public AlgoStrategy{
    public:
        KruskalAlgo(Graph& graph) : AlgoStrategy(graph) {}
        void execute() override {
            vector<pair<twoPoints,double>> mst = kruskal(g);
            cout << "Edges in the Minimum Spanning Tree (Kruskal's Algorithm):"<<endl;
            for(auto& edge: mst){
                cout << "Edge: (" << edge.first.first << ", " << edge.first.second << "), Weight: " << edge.second << endl;
            }
        }
};
class SCCAlgo: public AlgoStrategy{
    public:
        SCCAlgo(Graph& graph) : AlgoStrategy(graph) {}
        void execute() override {
            vector<vector<int>> scc = get_SCC(g);
            cout << "Strongly Connected Components:"<<endl;
            int count=1;
            for(auto& comp: scc){
                cout << "Component " << count++ << ": ";
                for(auto& v: comp){
                    cout << v << " ";
                }
                cout << endl;
            }
        }
};
class EdmondsKarpAlgo: public AlgoStrategy{
    public:
        EdmondsKarpAlgo(Graph& graph): AlgoStrategy(graph){}
        void execute() override{
            FlowNetwork fn = edmonds_carp(g);
            cout << "Max Flow from source to sink (Edmonds-Karp Algorithm):" << endl;
            double maxFlow = 0.0;
            for(int i=0;i<g.getVertices();++i){
                if(fn.isConnected(0,i)){
                    maxFlow += fn.getFlow(0,i);
                }
            }
            cout << "Maximum Flow: " << maxFlow << endl;
        }
};


