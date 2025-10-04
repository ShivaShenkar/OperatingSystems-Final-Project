#pragma once
#include "graphDS.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <stack>
#include <iostream>


/// Depth-first search helper (internal)
void dfs(const Graph& g, int v, std::vector<bool>& visited);

/// Checks if the graph is connected
bool isGraphConnected(const Graph& g);

/// Checks if the graph has an Eulerian circle
bool hasEulerianCircle(const Graph& g);

/// Finds an Eulerian circle (if one exists)
std::string findEulerianCircle(const Graph& g);
