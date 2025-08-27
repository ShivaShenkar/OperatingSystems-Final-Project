#include "euler.cpp"
int main(){
    //Example 1 
    {
        Graph g(3);
        g.addEdge(0,1,0);
        g.addEdge(1,2,0);
        g.addEdge(2,0,0);
        findEulerianCircle(g);
    }

    // Example 2
    {
        Graph g(4);
        g.addEdge(0,1,0);
        g.addEdge(1,2,0);
        g.addEdge(2,3,0);
        g.addEdge(3,0,0);
        g.addEdge(0,2,0);
        g.addEdge(1,3,0);
        findEulerianCircle(g);
    }

    // Example 3 (No Eulerian Circle)
    {
        Graph g(3);
        g.addEdge(0,1,0);
        g.addEdge(1,2,0);
        findEulerianCircle(g);
    }

    // Example 4 (No Eulerian Circle)
    {
        Graph g(4);
        g.addEdge(0,1,0);
        g.addEdge(2,3,0);
        findEulerianCircle(g);
    }
    //Example 5
    {
        Graph g(5);
        g.addEdge(0,1,0);
        g.addEdge(1,2,0);
        g.addEdge(2,0,0);
        g.addEdge(0,3,0);
        g.addEdge(3,4,0);
        g.addEdge(4,0,0);
        findEulerianCircle(g);
    }
}