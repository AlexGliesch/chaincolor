#pragma once
#include "util.h"
#include <queue>

/*
 * Structure representing a graph with attributes on the vertices
 */
struct Graph {
  // Read from a file with DIMACS format
  void ReadDimacs(const std::string& file);

  // Build graph from an adjacency list/matrix
  void BuildFromAdjacencyMatrix(const VVB& am);
  void BuildFromAdjacencyList(const VVI& al);

  // Checks if a vertex `v` has a neighbor satisfying a given condition `cond :
  // [n] -> bool`
  inline bool HasNb(int v, const auto& cond) const;

  // Iterator over edges in a graph
  struct EdgeIterator {
    EdgeIterator(int v, const Graph& g) : v(v), g(g) {}
    int v;

    const Graph& g;
    auto begin() { return g.eAdj.begin() + g.e0[v]; }
    auto end() { return g.eAdj.begin() + g.e0[v + 1]; }
  };

  // Returns an Edge iterator
  EdgeIterator Edges(int v) const { return EdgeIterator(v, *this); }

  // Runs a BFS on the graph, starting at node `start`. Only visits neighbors
  // for which `allow`:[n]->bool returns true. At each node visited, function
  // `visit`:[n]->void is executed. Returns the number of nodes visited, and, if
  // `dist` is not nullptr, stores the distance table there. [The lambdas here
  // may cause a small performance dip, watch out if it becomes an issue.]
  // clang-format off
  inline int BFS(int start, VI* dist, const auto& allow, const auto& visit) const;
  inline int BFS(int start, VI* dist, const auto& allow) const { return BFS(start, dist, allow, [](int) {}); }
  inline int BFS(int start, VI* dist) const { return BFS( start, dist, [](int) { return true; }, [](int) {}); } // clang-format on

  int n = 0; // Number of vertices
  int m = 0; // Number of edges
  int Delta = 0; // Largest degree in the graph
  VI deg; // Vertex degree
  VI eAdj; // Adjacency list of edges. Size: m*2
  VI e0; // Start index in E of each node. Size: n
};

// Implementations of inline functions
bool Graph::HasNb(int v, const auto& cond) const {
  for (const int j : Edges(v))
    if (cond(j)) return true;
  return false;
}

int Graph::BFS(int start, VI* dist, const auto& allow,
               const auto& visit) const {
  assert(start >= 0 and start <= n - 1);
  VI _dist;
  VI& dd = dist != nullptr ? *dist : _dist;
  std::queue<int> q;
  dd.assign(n, INT_MAX);
  dd[start] = 0;
  int numVis = 0;
  q.push(start);
  while (not q.empty()) {
    int v = q.front();
    q.pop();
    visit(v);
    ++numVis;
    for (const int j : Edges(v))
      if (j != -1 and allow(j) and dd[j] == INT_MAX)
        dd[j] = dd[v] + 1, q.push(j);
  }
  return numVis;
}