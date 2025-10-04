#include "algos.hpp"

// ----------------------
// Kruskal
// ----------------------
std::vector<std::pair<twoPoints,double>> kruskal(const Graph& g) {
    std::vector<std::pair<twoPoints,double>> mst;
    int n = g.getVertices();
    UnionFind uf(n);
    std::vector<std::tuple<double,int,int>> edges;

    for(int i=0;i<n;++i){
        for(int j=i+1;j<n;++j){
            if(g.isConnected(i,j)){
                edges.push_back({g.getWeight(i,j),i,j});
            }
        }
    }
    std::sort(edges.begin(), edges.end());
    for(auto& edge:edges){
        int u = std::get<1>(edge);
        int v = std::get<2>(edge);
        if(uf.find(u) != uf.find(v)){
            uf.unite(u,v);
            mst.push_back({{u,v},std::get<0>(edge)});
        }
    }
    return mst;
}

double MSTWeight(const std::vector<std::pair<twoPoints,double>> &mst) {
    double minWeight = 0;
    for(auto& edge:mst)
        minWeight += edge.second;
    return minWeight;
}

// ----------------------
// SCC
// ----------------------
std::vector<std::vector<int>> get_SCC(const Graph& g){
    std::vector<int> currVertices(g.getVertices());
    std::iota(currVertices.begin(), currVertices.end(), 0);
    std::vector<std::vector<int>> scc;
    std::vector<bool> visited(g.getVertices(),false);

    while(!currVertices.empty()){
        dfs(g,currVertices[0],visited);
        std::vector<int> comp;
        for(auto& v: currVertices){
            if(visited[v]){
                comp.push_back(v);
                currVertices.erase(std::remove(currVertices.begin(), currVertices.end(), v), currVertices.end());
            }
        }
        if(!comp.empty()){
            scc.push_back(comp);
        }
    }
    return scc;
}

bool isPathConnected(const Graph& g, int i,int j){
    std::vector<bool> visited(g.getVertices(),false);
    dfs(g,i,visited);
    return visited[j];
}

// ----------------------
// Flow Network
// ----------------------
struct FlowEdge{
    double capacity;
    double flow;
    double residualCapacity() const { return capacity - flow; }
};

FlowNetwork::FlowNetwork(const Graph& g) : n(g.getVertices()) {
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

FlowNetwork::~FlowNetwork(){
    for(int i=0;i<n;++i){
        delete[] edges[i];
        delete[] f_edges[i];
    }
    delete[] edges;
    delete[] f_edges;
}

bool FlowNetwork::isConnected(int i,int j) const {
    return (i!=j&&i>=0&&j>=0&&i<n&&j<n) ? edges[i][j] : false;
}
int FlowNetwork::getVertices() const { return n; }
double FlowNetwork::getresidualCapacity(int i,int j) const {
    if(isConnected(i,j)) return f_edges[i][j].residualCapacity();
    throw ("invalid vertices\nno edge between the two vertices");
}
double FlowNetwork::getFlow(int i,int j) const {
    if(isConnected(i,j)) return f_edges[i][j].flow;
    throw ("invalid vertices\nno edge between the two vertices");
}
double FlowNetwork::getCapacity(int i,int j) const {
    if(isConnected(i,j)) return f_edges[i][j].capacity;
    throw ("invalid vertices\nno edge between the two vertices");
}
void FlowNetwork::addFlow(int i,int j, double amount){
    if(isConnected(i,j)){
        if(f_edges[i][j].residualCapacity()>=amount){
            f_edges[i][j].flow += amount;
        } else {
            throw ("not enough residual capacity");
        }
    } else throw ("invalid vertices\nno edge between the two vertices");
}

// ----------------------
// Edmonds-Karp
// ----------------------
std::vector<std::pair<twoPoints,int>> findAugmentingPath(const FlowNetwork& fn){
    int n = fn.getVertices();
    std::vector<bool> visited(n,false);
    std::vector<int> parentType(n,0);
    std::vector<int> parent(n,-1);
    std::queue<int> q;
    bool reached = false;
    visited[0] = true;
    q.push(0);

    while (!q.empty() && !reached) {
        int u = q.front(); q.pop();
        for(int i=0;i<n;++i){
            bool b1 = fn.isConnected(u,i), b2 = fn.isConnected(i,u);
            int isForward;
            double maxResCap;
            if((b1||b2) && !visited[i]){
                if(b1&&b2){ maxResCap = fn.getresidualCapacity(u,i)+fn.getFlow(i,u); isForward=2; }
                else if(b1){ maxResCap = fn.getresidualCapacity(u,i); isForward=1; }
                else{ maxResCap = fn.getFlow(i,u); isForward=0; }
                if(maxResCap>0){
                    parent[i] = u;
                    visited[i] = true;
                    parentType[i] = isForward;
                    q.push(i);
                    if(i==n-1){ reached=true; break; }
                }
            }
        }
        if(!reached && q.empty()) return {};
    }

    std::vector<std::pair<twoPoints,int>> res;
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
        std::cout<<"No path from source to sink"<<std::endl;
        return fn;
    }
    while(true){
        auto augPath = findAugmentingPath(fn);
        if(augPath.empty()) break;
        double minResCap = std::numeric_limits<double>::max();
        for(auto& [edge,isForward]: augPath){
            if(isForward) minResCap = std::min(minResCap,fn.getresidualCapacity(edge.second,edge.first));
            else minResCap = std::min(minResCap,fn.getFlow(edge.first,edge.second));
        }
        for(auto& [edge,isForward]: augPath){
            if(isForward) fn.addFlow(edge.second,edge.first,minResCap);
            else fn.addFlow(edge.first,edge.second,-minResCap);
        }
    }
    return fn;
}

// ----------------------
// Max Clique
// ----------------------
std::vector<int> max_clique(const Graph& g){
    int n = g.getVertices();
    std::vector<int> maxClique;
    for(int i=(1<<n)-1;i>=1;--i){
        std::vector<int> currClique;
        bool isClique = true;
        for(int j=0;j<n;++j){
            if(i&(1<<j)){
                for(int k=0;k<(int)currClique.size();++k){
                    if(!g.isConnected(j,currClique[k])){ isClique = false; break; }
                }
                if(!isClique) break;
                currClique.push_back(j);
            }
        }
        if(isClique && currClique.size()>maxClique.size()){
            maxClique = currClique;
        }
    }
    return maxClique;
}

// ----------------------
// Strategy Pattern
// ----------------------
AlgoStrategy::AlgoStrategy(Graph& graph) : g(graph) {}

KruskalAlgo::KruskalAlgo(Graph& graph) : AlgoStrategy(graph) {}
std::string KruskalAlgo::execute() {
    auto mst = kruskal(g);
    std::ostringstream oss;
    oss << "[";
    for(size_t i=0; i<mst.size(); ++i){
        oss << "((" << mst[i].first.first << "," << mst[i].first.second << "), " << mst[i].second << ")";
        if(i != mst.size()-1) oss << ", ";
    }
    oss << "]";
    return oss.str();
}

SCCAlgo::SCCAlgo(Graph& graph) : AlgoStrategy(graph) {}
std::string SCCAlgo::execute() {
    auto scc = get_SCC(g);
    std::ostringstream oss;
    oss << "[";
    for(size_t i=0; i<scc.size(); ++i){
        oss << "[";
        for(size_t j=0; j<scc[i].size(); ++j){
            oss << scc[i][j];
            if(j != scc[i].size()-1) oss << ", ";
        }
        oss << "]";
        if(i != scc.size()-1) oss << ", ";
    }
    oss << "]";
    return oss.str();
}

EdmondsKarpAlgo::EdmondsKarpAlgo(Graph& graph) : AlgoStrategy(graph) {}
std::string EdmondsKarpAlgo::execute() {
    FlowNetwork fn = edmonds_carp(g);
    double maxFlow = 0.0;
    for(int i=0;i<g.getVertices();++i){
        if(fn.isConnected(0,i)) maxFlow += fn.getFlow(0,i);
    }
    std::ostringstream oss;
    oss << maxFlow;
    return oss.str();
}

MaxCliqueAlgo::MaxCliqueAlgo(Graph& graph) : AlgoStrategy(graph) {}
std::string MaxCliqueAlgo::execute() {
    auto clique = max_clique(g);
    std::ostringstream oss;
    oss << "[";
    for(size_t i=0; i<clique.size(); ++i){
        oss << clique[i];
        if(i != clique.size()-1) oss << ", ";
    }
    oss << "]";
    return oss.str();
}

AlgoStrategy* AlgoFactory::createAlgo(std::string& algoName, Graph& g) {
    std::string lower = algoName;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower == "mst") return new KruskalAlgo(g);
    if (lower == "clique") return new MaxCliqueAlgo(g);
    if (lower == "flow") return new EdmondsKarpAlgo(g);
    if (lower == "scc") return new SCCAlgo(g);

    throw std::invalid_argument("Unknown algorithm: " + algoName);
}

