#pragma once
#include <numeric>  // for std::iota
#include <algorithm> // for std::swap
#include <vector>

// No using namespace std in headers

struct UnionFind {
    std::vector<int> parent;
    std::vector<int> rank;

    /// Constructor: initializes n disjoint sets
    UnionFind(int n);

    /// Finds the representative of a set with path compression
    int find(int v);

    /// Unites the sets containing a and b
    bool unite(int a, int b);
};
