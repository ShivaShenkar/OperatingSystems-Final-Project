#pragma once
#include "graphDS.hpp"
#include "union_find.hpp"
#include "euler.hpp"
#include <algorithm>
#include <limits>
#include <queue>
#include <numeric>
#include <tuple>
#include <sstream>
#include <iostream>
#include <vector>
#include <utility>
#include <string>

// alias
using twoPoints = std::pair<int,int>;

// ------------------ Algorithms ------------------
std::vector<std::pair<twoPoints,double>> kruskal(const Graph& g);
double MSTWeight(const std::vector<std::pair<twoPoints,double>>& mst);

std::vector<std::vector<int>> get_SCC(const Graph& g);
bool isPathConnected(const Graph& g, int i, int j);

class FlowNetwork; // forward declaration
std::vector<std::pair<twoPoints,int>> findAugmentingPath(const FlowNetwork& fn);
FlowNetwork edmonds_carp(const Graph& g);

std::vector<int> max_clique(const Graph& g);

// ------------------ Flow Network ------------------
struct FlowEdge;
class FlowNetwork {
private:
    bool** edges;
    FlowEdge** f_edges;
    int n;
public:
    FlowNetwork(const Graph& g);
    ~FlowNetwork();

    bool isConnected(int i, int j) const;
    int getVertices() const;
    double getresidualCapacity(int i, int j) const;
    double getFlow(int i, int j) const;
    double getCapacity(int i, int j) const;
    void addFlow(int i, int j, double amount);
};

// ------------------ Strategy Pattern ------------------
class AlgoStrategy {
protected:
    Graph& g;
    AlgoStrategy(Graph& graph);
public:
    virtual ~AlgoStrategy() = default;
    virtual std::string execute() = 0;
};

class KruskalAlgo : public AlgoStrategy {
public:
    KruskalAlgo(Graph& graph);
    std::string execute() override;
};

class SCCAlgo : public AlgoStrategy {
public:
    SCCAlgo(Graph& graph);
    std::string execute() override;
};

class EdmondsKarpAlgo : public AlgoStrategy {
public:
    EdmondsKarpAlgo(Graph& graph);
    std::string execute() override;
};

class MaxCliqueAlgo : public AlgoStrategy {
public:
    MaxCliqueAlgo(Graph& graph);
    std::string execute() override;
};
class AlgoFactory{
    public:
        static AlgoStrategy* createAlgo(const std::string& algoName, Graph& g);
};
