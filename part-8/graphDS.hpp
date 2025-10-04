#pragma once
#include <exception>
#include <iostream>
class Graph{
    private:
        int n;
        bool **edges;
        double **weights;
    public:
        Graph(int vertices);
        Graph (const Graph& g);
        ~Graph();
        Graph& operator=(const Graph& g);
        int getVertices() const;
        bool isConnected(int i,int j) const;
        double getWeight(int i,int j) const;
        void addEdge(int i,int j,double weight);
        void addDirectedEdge(int i,int j,double weight);
        void removeEdge(int i,int j);
        int getDegree(int v) const;
};
