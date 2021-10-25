#include "graph.h"
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <sstream>

void Graph::BuildFromAdjacencyList(const VVI& al) {
  n = al.size();
  if (n == 0) return;
  m = 0;
  for (int i = 0; i < n; ++i) m += al[i].size();
  m /= 2;
  eAdj.resize(m * 2);
  e0.resize(n + 1);
  int count = 0;
  e0[0] = 0;
  Delta = 0;
  deg.resize(n);
  for (int i = 0; i < n; ++i) {
    for (int j : al[i]) eAdj[count++] = j;
    assert(count - e0[i] == (int)al[i].size());
    deg[i] = count - e0[i];
    Delta = std::max(Delta, deg[i]);
    e0[i + 1] = count;
  }
}

void Graph::BuildFromAdjacencyMatrix(const VVB& am) {
  n = am.size();
  if (n == 0) return;
  assert(am[0].size() == am.size());
  m = 0;
  for (int i = 0; i < n; ++i)
    for (int j = i + 1; j < n; ++j) m += am[i][j];
  eAdj.resize(m * 2);
  e0.resize(n + 1);
  int count = 0;
  e0[0] = 0;
  Delta = 0;
  deg.resize(n);
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j)
      if (am[i][j]) eAdj[count++] = j;

    deg[i] = count - e0[i];
    Delta = std::max(Delta, deg[i]);
    e0[i + 1] = count;
  }
}

void Graph::ReadDimacs(const std::string& file) {
  std::ifstream f(file);
  if (not f.good() or f.fail()) {
    pr("(!) Error: could not open input file.\n");
    exit(EXIT_FAILURE);
  }
  std::string buf, tmp;
  m = n = 0;
  bool invalid = false;
  VVI al;
  while (not invalid and getline(f, buf)) {
    boost::trim_left(buf);
    if (buf.empty()) continue;
    if (buf[0] == 'c') {
      continue; // comment
    } else if (buf[0] == 'p') {
      std::stringstream ss(buf);
      ss >> tmp >> tmp >> n >> m;
      if (boost::to_lower_copy(tmp) != "edge" and
          boost::to_lower_copy(tmp) != "col") {
        invalid = true;
        break;
      }
      al.resize(n);
    } else if (buf[0] == 'e') {
      std::stringstream ss(buf);
      int v1, v2;
      ss >> tmp >> v1 >> v2;
      assert(InRange(v1, 1, n) and InRange(v2, 1, n));
      --v1, --v2;
      if (not LinearIn(al[v1], v2)) {
        assert(not LinearIn(al[v2], v1));
        al[v1].push_back(v2);
        al[v2].push_back(v1);
        ++m;
      }
    } else
      invalid = true;
  }
  if (invalid) {
    pr("(!) Error: instance {} does not seem to be in DIMACS col format.\n",
       file);
    pr("(!) Line: {}\n", buf);
    exit(EXIT_FAILURE);
  }
  BuildFromAdjacencyList(al);
}