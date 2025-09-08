#include <vector>
#include <numeric>
using namespace std;
struct UnionFind {
    vector<int> parent, rank;

    UnionFind(int n) {
        parent.resize(n);
        rank.assign(n,0);
        iota(parent.begin(), parent.end(), 0); 
    }

    int find(int v) {
        if (v == parent[v]) return v;
        return parent[v] = find(parent[v]); 
    }

    bool unite(int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b) return false;
        if (rank[a] < rank[b]) swap(a, b);
        parent[b] = a;
        if (rank[a] == rank[b]) rank[a]++;
        return true;
    }
};