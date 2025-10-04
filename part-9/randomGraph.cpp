#include "randomGraph.hpp"

Graph generateRandomGraph(int v, int e, int s) {
    // Seed handling
    if (s == -1) {
        s = static_cast<int>(std::chrono::system_clock::now().time_since_epoch().count());
    }

    std::mt19937 rng(s);
    std::uniform_int_distribution<int> dist(0, v - 1);

    std::set<std::pair<int,int>> currEdges;
    Graph g(v);

    while (static_cast<int>(currEdges.size()) < e) {
        int u = dist(rng);
        int w = dist(rng);
        if (u != w) {
            auto edge = std::minmax(u, w); // undirected edge, sorted
            if (currEdges.insert(edge).second) {
                g.addEdge(u, w, 0);
            }
        }
    }

    return g;
}
