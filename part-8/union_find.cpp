#include "union_find.hpp"


UnionFind::UnionFind(int n) {
    parent.resize(n);
    rank.assign(n, 0);
    std::iota(parent.begin(), parent.end(), 0);
}

int UnionFind::find(int v) {
    if (v == parent[v]) return v;
    return parent[v] = find(parent[v]);
}

bool UnionFind::unite(int a, int b) {
    a = find(a);
    b = find(b);
    if (a == b) return false;
    if (rank[a] < rank[b]) std::swap(a, b);
    parent[b] = a;
    if (rank[a] == rank[b]) rank[a]++;
    return true;
}
