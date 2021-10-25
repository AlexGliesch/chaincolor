#pragma once
#include "graph.h"
#include "util.h"

/*
 * Main algorithm
 * */
struct ChainColor {
  ChainColor(const Graph& g);

  // Run algorithm
  int Color(VI& color, Timer t);

private:
  //
  void _Color(VI& color);

  //
  bool Chain(const int v, VI& color, VB& fixed, const int depth,
             const int branches);

  Graph g; // input graph
  int k = 0; // current number of colors

  // Algorithm options
  int p1 = INT_MAX; // if depth >= p1, only expand OR nodes with 1 AND-branch
  int p2 = INT_MAX; // if depth >= p2, only expand on the first OR node
  int p3 = INT_MAX; // if depth >= p3, only expand OR nodes which  haven't
                    // failed in the past

  int maxDepth = INT_MAX; // if depth >= maxDepth, stop
  int maxAndWidth = INT_MAX; // prune nodes with AND width greater than this

  int maxAndBranches = INT_MAX; //

  // Internal variables
  int removedColor = -1; // color we're trying to remove in this Chain recursion
  VI andWidth; // stores width of neighbor AND nodes during  recursion
  VI andColors; // used to sort neighbor AND nodes during recursion
  VI2 tried, failed; // numbers of tries/fails to assign vertices to colors
};