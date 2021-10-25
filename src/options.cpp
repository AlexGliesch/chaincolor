#include "options.h"
#include "random.h"
#include "util.h"
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <set>
#include <sys/ioctl.h>

namespace opt {

void ReadCmdLine(int argc, char** argv) {
  namespace po = boost::program_options;
  struct winsize wsize;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &wsize);

  // clang-format off
  po::options_description desc("General options", wsize.ws_col);
  desc.add_options()("help", "Show help");
  desc.add_options()("seed,s", po::value<uint64_t>(&seed)->default_value(seed),
                     "Random seed. If 0, a seed will be selected based on system time and the process ID");
  desc.add_options()("verbose,v", po::value(&verbosec)->zero_tokens(),
                     "Verbosity. If present, output is sent to screen. If -v is repeated, more output is given");
  desc.add_options()("time,t", po::value<double>(&timeLimit)->default_value(timeLimit),
                     "Time limit (seconds)");
  desc.add_options()("in,i", po::value<std::string>(&instancePath)->required(), "Instance file");

  // clang-format on
  po::options_description all("", wsize.ws_col);
  all.add(desc);
  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, all), vm);
    po::notify(vm);
    if (vm.count("help")) {
      std::cout << all << std::endl;
      exit(10);
    }
    // Validate parameters
    [[maybe_unused]] auto validate =
        [&](const std::string& name, std::initializer_list<std::string> opts) {
          auto val = vm[name].as<std::string>();
          if (not std::set(opts).contains(val))
            throw po::error(
                fmt::format("invalid value (\"{}\") for option {}", val, name));
        };
    //    validate("alg", {"", "default"});
  } catch (po::error& e) {
    if (not vm.count("help"))
      pr("\n(!) Error parsing command line: {}.\n", e.what());
    std::cout << all << std::endl;
    exit(EXIT_FAILURE);
  }

  Random::SetSeed(seed);
}

} // namespace opt
