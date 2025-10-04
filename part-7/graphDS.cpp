#include "graphDS.hpp"

using namespace std;

Graph::Graph(int vertices){
    if(vertices<=0)
        throw invalid_argument("Invalid number of vertices");
    this->n = vertices;
    this->edges = new bool*[n];
    this->weights = new double*[n];
    for(int i=0;i<n;i++){
        edges[i] = new bool[n];
        weights[i] = new double[n];
        for(int j=0;j<n;j++){
            edges[i][j]=false;
        }
        
    }
}
Graph::~Graph(){
    for(int i=0;i<n;i++){
        delete[] edges[i];
        delete[] weights[i];
    }
    delete[] edges;
    delete[] weights;
}
bool Graph::isConnected(int i,int j) const{
    if(i>=0&&j>=0&&i<n&&j<n){
        return edges[i][j];
    }
    return false;
}
double Graph::getWeight(int i,int j) const{
    if(isConnected(i,j))
        return weights[i][j];
    throw ("invalid vertices\no edge between the two vertices");
    
}
int Graph::getVertices()const {return n;}
Graph::Graph(const Graph& g){
    this->n = g.getVertices();
    this->edges = new bool*[n];
    this->weights = new double*[n];
    for(int i=0;i<n;i++){
        edges[i] = new bool[n];
        weights[i] = new double[n];
    }
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            edges[i][j] = g.isConnected(i,j);
            if(edges[i][j])
                weights[i][j] = g.getWeight(i,j);
        }
    }
}
Graph& Graph::operator=(const Graph& g){
    if(this == &g)
        return *this;
    // Free existing memory
    for(int i=0;i<n;i++){
        delete[] edges[i];
        delete[] weights[i];
    }
    delete[] edges;
    delete[] weights;
    // Copy new size
    n = g.getVertices();
    // Allocate new memory
    edges = new bool*[n];
    weights = new double*[n];
    for(int i=0;i<n;i++){
        edges[i] = new bool[n];
        weights[i] = new double[n];
    }
    // Deep copy contents
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            edges[i][j] = g.isConnected(i,j);
            weights[i][j] = g.getWeight(i,j);
        }
    }
    return *this;
}

void Graph::addEdge(int i,int j,double weight){
    if(i!=j&&i>=0&&j>=0&&i<n&&j<n){
        edges[i][j] = true;
        edges[j][i] = true;
        weights[i][j] = weight;
        weights[j][i] = weight;
    }
}
void Graph::addDirectedEdge(int i,int j,double weight){
    if(i!=j&&i>=0&&j>=0&&i<n&&j<n){
        edges[i][j] = true;
        weights[i][j] = weight;;
    }
}
void Graph::removeEdge(int i,int j){
    if(i!=j&&i>=0&&j>=0&&i<n&&j<n){
        edges[i][j] = false;
        edges[j][i] = false;
    }
}

int Graph::getDegree(int v) const {
        int deg = 0;
        for (int i = 0; i < n; ++i){
            deg += (this->edges[v][i]) ? 1:0;
        }
        return deg;
}
