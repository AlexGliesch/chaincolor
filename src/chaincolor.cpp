#include "chaincolor.h"
#include "colorutil.h"
#include "random.h"

// TODO limit total number of AND branches?

// TODO limit maximum AND branch width? Increase this limit recursively?
// TODO if recursive, can cache the failures

// TODO make sure this removes trivial (and trivial[1]) colors without overhead
// TODO over the basic method

// TODO a node which failed to be recolored in some branch, is very likely to
// TODO fail again in another branch

// TODO what to do with p1, p2, p3?

ChainColor::ChainColor(const Graph& g) : g(g), k(g.n) {}

int ChainColor::Color(VI& color, Timer t) {
  k = DSatur(g, color); // initial greedy solution
  AssertCorrect(g, color, k);
  pr("DSatur k: {}, time: {}\n", k, t.Elapsed());

  //  t.Reset();
  for (int i = 1; i <= 7 and not t.TimedOut(); ++i) {
    p1 = i;
    p2 = i * 2;
    //    p3 = i * 2;
    //  for (int i = 1; not t.TimedOut(); i = i * 2) {
    //    maxAndBranches = i;
    _Color(color);
    pr("ChainColor done; p1: {}, maxAndBranches: {}, k: {}, time: {}\n", p1,
       maxAndBranches, k, t.Elapsed());
  }
  return k;
}

void ChainColor::_Color(VI& color) {
  pr("ChainColor; p1: {}, maxAndBranches: {}, k: {}\n", p1, maxAndBranches, k);
  AssertCorrect(g, color, k);

  maxDepth = std::min(maxDepth, g.n + 1);
  maxAndWidth = std::min(maxAndWidth, g.Delta + 1);

  // Allocate helper memory
  andColors.resize(k * maxDepth);
  andWidth.resize(k * maxDepth);

  VI numTrivial, trivial, size;
  ComputeTrivialVertices(g, color, k, size, numTrivial, trivial);

  VI cols{Iota(k)};
  std::sort(cols.begin(), cols.end(), [&](int c1, int c2) {
    return size[c1] - numTrivial[c1] < size[c2] - numTrivial[c2];
  });

  VB fixed(g.n, false);

  // Try all colors
  for (int c : cols) {
    pr(1, "Try color {} size {} trivial {} non-trivial {}\n", c, size[c],
       numTrivial[c], size[c] - numTrivial[c]);
    VI _color{color};

    // Immediately switch all trivials
    for (int v = 0; v < g.n; ++v)
      if (_color[v] == c and trivial[v] != -1) _color[v] = trivial[v];

    // Chain all non-trivials
    bool ok = true;
    for (int v = 0; v < g.n; ++v)
      if (_color[v] == c and trivial[v] == -1) {
        //        pr(2, "Chaining vertex {}...\n", v);
        [[maybe_unused]] VI __color{_color};
        failed.assign(g.n, k, 0);
        tried.assign(g.n, k, 0);
        removedColor = c;
        ok = Chain(v, _color, fixed, 0, 0);
        assert(not LinearIn(fixed, true));
        if (not ok) {
          pr(2, "Failed to chain vertex {}\n", v);
          break;
        }
        [[maybe_unused]] int chainLength = 0;
        for (int u = 0; u < g.n; ++u) chainLength += (_color[u] != __color[u]);
        pr(2, "Chained vertex {}, length: {}\n", v, chainLength);
        assert(chainLength > 0);
      }
    // Ok, all vertices were chained. Remove color
    if (ok) {
      pr(1, "Removing color {}\n", c);
      [[maybe_unused]] int kBefore = k;
      RearrangeColors(_color, k);
      assert(k < kBefore);
      color = std::move(_color);
      return _Color(color); // Recursive call to keep chaining
    }
  }
}

bool ChainColor::Chain(const int v, VI& color, VB& fixed, const int depth,
                       const int branches) {
  assert(not fixed[v]);

  // Determine and-width of each color, and list `ac` of colors which will be
  // iterated over
  int sz = 0, *aw = &andWidth[depth * k], *ac = &andColors[depth * k];
  std::fill(aw, aw + k, 0);
  for (const int u : g.Edges(v)) {
    int c = color[u];
    if (aw[c] == -1) continue;
    if ( //
        fixed[u] //
        or (failed[v][c] > 0 and depth >= p3) //
        or c == removedColor //
        or c == color[v] //
        //        or Random::RandInt(-tried[v][c], tried[v][c]) >
        //               tried[v][c] - failed[v][c] //
    )
      aw[c] = -1;
    else
      ++aw[c];
  }
  for (int c = 0; c < k; ++c) {
    if (c == removedColor or c == color[v]) continue;
    if (aw[c] == 0) { // Can trivially change to color c. Do it
      color[v] = c;
      return true;
    }
    if (aw[c] > 0 and aw[c] <= maxAndWidth and (depth < p1 or aw[c] <= 1) and
        aw[c] - 1 + branches <= maxAndBranches) {
      ac[sz++] = c;
    }
  }

  // Don't branch if will exceed maximum depth, or no OR-nodes to branch
  if (depth + 1 >= maxDepth or sz == 0) return false;

  // Iterate over colors, in order of least->most AND-width
  // (OR)
  std::sort(ac, ac + sz, [&](int c1, int c2) {
    return aw[c1] < aw[c2] or
           (aw[c1] == aw[c2] and
            failed[v][c1] * tried[v][c2] > failed[v][c2] * tried[v][c1]);
  });

  for (int i = 0; i < (depth >= p2 ? 1 : sz); ++i) {
    int c = ac[i];

    VI color_{color};
    VB fixed_{fixed};
    color_[v] = c;
    fixed_[v] = true;
    bool ok = true;
    ++tried[v][c];

    // Chain all neighbors [order?]
    // (AND)
    for (const int u : g.Edges(v)) // TODO order!
      if (color_[u] == c) {
        ok = ok and Chain(u, color_, fixed_, depth + 1, branches + aw[c] - 1);
        if (not ok) break;
      }
    if (ok) {
      color = std::move(color_);
      fixed = std::move(fixed_);
      fixed[v] = false;
      return true; // Ok, all neighbors changed color, v is feasible
    } else {
      ++failed[v][c];
    }
  }
  return false;
}
