#include "chaincolor.h"
#include "options.h"
#include <filesystem>

int main(int argc, char** argv) {
  opt::ReadCmdLine(argc, argv);
  Graph g;
  g.ReadDimacs(opt::instancePath);

  std::string instName(std::filesystem::path(opt::instancePath).stem());

  pr("name: {}\n", instName);
  pr("n: {}\n", g.n);
  pr("m: {}\n", g.m);
  pr("Delta: {}\n", g.Delta);

  Timer t(opt::timeLimit);
  VI color;
  int k = ChainColor(g).Color(color, t);
  return k;
}