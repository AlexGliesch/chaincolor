#pragma once
#include "graph.h"
#include "util.h"

/*
 * Utilitary functions/heuristics related to coloring
 * */

// Given a coloring 'color' with 'k' colors, computes:
// size: for each color, the number of nodes
// numTrivial: for each color, the number of trivial nodes
// trivial: for each vertex, -1 if it's not trivial, else a color it could be
//          changed into
void ComputeTrivialVertices(const Graph& g, const VI& color, const int k,
                            VI& size, VI& numTrivial, VI& trivial);

// Makes sure all colors are within [0, numColors). Reassigns k, if needed.
void RearrangeColors(VI& color, int& k);

// Given a coloring 'color' with 'k' colors, removes trivial colors.
void RemoveTrivialColors(const Graph& g, VI& color, int& k);

// Tries out 'iter' random greedy orderings. Returns the number of colors, and
// coloring in 'out'.
int GreedyColor(const Graph& g, VI& out, const int iter);

// DSatur order for greedy. Returns the number of colors, and coloring in 'out'.
int DSatur(const Graph& g, VI& out);

// Sanity check on a coloring.
void AssertCorrect(const Graph& g, const VI& color, const int k);