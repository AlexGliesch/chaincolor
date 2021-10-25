#include "colorutil.h"
#include "random.h"

void ComputeTrivialVertices(const Graph& g, const VI& color, const int k,
                            VI& size, VI& numTrivial, VI& trivial) {
  static VI used; // helper memory to check which colors are used at each vertex
                  // iterated
  used.assign(k, 0);
  size.assign(k, 0);
  numTrivial.assign(k, 0);
  trivial.assign(g.n, -1);
  for (int i = 0; i < g.n; ++i) {
    ++size[color[i]];
    for (const int j : g.Edges(i)) used[color[j]] = i;
    for (int c = 0; c < k; ++c)
      if (c != color[i] and used[c] != i) {
        ++numTrivial[color[i]];
        trivial[i] = c;
        break;
      }
  }
}

void RearrangeColors(VI& color, int& k) {
  VI nc(k, -1);
  k = 0;
  for (size_t i = 0; i < color.size(); ++i) {
    if (nc[color[i]] == -1) nc[color[i]] = k++;
    color[i] = nc[color[i]];
  }
}

void RemoveTrivialColors(const Graph& g, VI& color, int& k) {
  assert(k - 1 == *std::max_element(color.begin(), color.end()));
  static VI numTrivial, trivial, size;
  ComputeTrivialVertices(g, color, k, size, numTrivial, trivial);

  // If any color is trivial (i.e., has all trivial vertices), remove that
  // color
  for (int c = 0; c < k; ++c) {
    if (size[c] > 0 and size[c] == numTrivial[c]) {
      // Remove trivial color c
      for (int i = 0; i < g.n; ++i)
        if (color[i] == c) {
          assert(trivial[i] != -1);
          color[i] = trivial[i];
        }
      RearrangeColors(color, k);
      return RemoveTrivialColors(g, color, k); // Keep removing colors
    }
  }
}

int DSatur(const Graph& g, VI& color) {
  VB2 used(g.n, g.Delta + 1, false);
  VI unqNbCols(g.n, 0), deg(g.deg);
  color.assign(g.n, -1);
  int k = 0;
  int v = std::max_element(g.deg.begin(), g.deg.end()) - g.deg.begin();
  for (int it = 0; it < g.n; ++it) {
    for (int c = 0;; ++c)
      if (not used[v][c]) {
        color[v] = c;
        k = std::max(k, c + 1);
        break;
      }
    for (int u : g.Edges(v)) {
      --deg[u];
      unqNbCols[u] += (used[u][color[v]] == false);
      used[u][color[v]] = true;
    }
    for (int i = 0; i < g.n; ++i)
      if (color[i] == -1 and
          (color[v] != -1 or //
           std::tie(unqNbCols[i], deg[i]) > std::tie(unqNbCols[v], deg[v])))
        v = i;
  }
  assert(std::count(color.begin(), color.end(), -1) == 0);
  RemoveTrivialColors(g, color, k);
  return k;
}

int GreedyColor(const Graph& g, VI& out, const int maxIter) {
  VI order{Iota(g.n)};
  int K = g.Delta + 1;
  for (int iter = 0; iter < maxIter; ++iter) {
    static VI used, color;
    std::shuffle(order.begin(), order.end(), Random::rng);
    color.assign(g.n, -1);
    used.assign(g.Delta + 1, 0);
    int k = 0, it = 0;
    for (int i : order) {
      ++it;
      for (int j : g.Edges(i))
        if (color[j] != -1) used[color[j]] = it;
      for (int c = 0;; ++c)
        if (used[c] != it) {
          color[i] = c;
          k = std::max(k, c + 1);
          break;
        }
    }
    RemoveTrivialColors(g, color, k);
    if (k < K) {
      K = k;
      out = color;
    }
  }
  return K;
}

void AssertCorrect(const Graph& g, const VI& color, const int k) {
  assert(color.size() == size_t(g.n));
  int maxColor = 0;
  for (int v = 0; v < g.n; ++v) {
    maxColor = std::max(maxColor, color[v]);
    assert(InRange(color[v], 0, k - 1));
    for (int u : g.Edges(v)) assert(color[u] != color[v]);
  }
  assert(maxColor <= k - 1);
}
