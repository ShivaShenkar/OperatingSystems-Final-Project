#pragma once
#include "../part-1/graphDS.hpp"
#include <vector>
#include <stack>
#include <iostream>
using namespace std;

void dfs(const Graph& g,int v, vector<bool>& visited) {
    visited[v] = true;
    for (int i = 0; i < g.getVertices(); ++i){
        if (g.isConnected(v,i) && !visited[i])
            dfs(g,i, visited);
    }
}
bool isGraphConnected(const Graph& g) {
    vector<bool> visited(g.getVertices(), false);
    int start = -1;
    for (int i = 0; i < g.getVertices(); ++i) {
        if (g.getDegree(i) > 0) {
            start = i;
            break;
        }
    }
    if (start == -1){
        if(g.getVertices()==1)
            return true;
        return false;
    }
    dfs(g,start, visited);
    for (int i = 0; i < g.getVertices(); ++i)
        if (g.getDegree(i) > 0 && !visited[i])
            return false;
    return true;
}
bool hasEulerianCircle(const Graph& g) {
    if (!isGraphConnected(g)){
        cout<<"Graph is not connected"<<endl;
        return false;
    }
    for (int i = 0; i < g.getVertices(); ++i){
        if (g.getDegree(i) % 2 != 0){
            cout<<"The vertex "<<i<<" has odd degree"<<endl;
            return false;
        }
    }
    return true;
}

string findEulerianCircle(const Graph& g) {
    Graph newGraph = g;
    if (!hasEulerianCircle(newGraph)) {
        cout << "No euler circle in the Graph" << endl;
        return "No euler circle in the Graph";
    }
    stack<int> currPath;
    vector<int> circle;
    int curr = 0;
    for (int i = 0; i < newGraph.getVertices(); ++i)
        if (newGraph.getDegree(i) > 0) {
            curr = i;
            break;
        }
    currPath.push(curr);
    while (!currPath.empty()) {
        if (newGraph.getDegree(curr) > 0) {
            currPath.push(curr);
            for (int next = 0; next < newGraph.getVertices(); ++next) {
                if (newGraph.isConnected(next,curr)) {
                    newGraph.removeEdge(next,curr);
                    curr = next;
                    break;
                }
            }
        } 
        else {
            circle.push_back(curr);
            curr = currPath.top();
            currPath.pop();
        }
    }
    string msg = "Eulerian Circle: ";
    cout << msg ;
    for (auto it = circle.rbegin(); it != circle.rend(); ++it){
        cout << *it << " ";
        msg+= to_string(*it)+ " ";
    }
    cout << endl;
    return msg;
}
