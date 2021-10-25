#pragma once
#include <array>
#include <boost/any.hpp>
#include <boost/multi_array.hpp>
#include <cfloat>
#include <chrono>
#include <climits>
#include <cmath>
#include <fmt/color.h>
#include <fmt/printf.h>
#include <iostream>
#include <memory>
#include <numeric>
#include <type_traits>

#if defined(_MSC_VER)
#define NOMINMAX
#endif

/*
 * Aliases
 */
using II = std::pair<int, int>;
using DI = std::pair<double, int>;
using ID = std::pair<int, double>;
using DD = std::pair<double, double>;
using VD = std::vector<double>;
using VI = std::vector<int>;
using VB = std::vector<bool>;
using VS = std::vector<std::string>;
using VII = std::vector<II>;
using VVI = std::vector<VI>;
using VVB = std::vector<VB>;
using III = std::tuple<int, int, int>;
using VIII = std::vector<III>;
using VVD = std::vector<VD>;
using VVI = std::vector<VI>;
using VDI = std::vector<DI>;
using VID = std::vector<ID>;
using VDD = std::vector<DD>;
using VVDI = std::vector<VDI>;
using NLD = std::numeric_limits<double>;
using NLI = std::numeric_limits<int>;

/*
 * Time-tracking structure.
 */
struct Timer {
  using clock = std::chrono::steady_clock;
  // Initialize a timer with a given time limit
  Timer(double tl_sec = DBL_MAX) { Reset(tl_sec); }
  // Resets a timer with a given time limit
  void Reset(double tl_sec = DBL_MAX) {
    tmlim = tl_sec, tpstart = clock::now();
  }
  // Elapsed seconds since the start
  double Elapsed() const {
    using namespace std::chrono;
    return duration_cast<duration<double>>(clock::now() - tpstart).count();
  }
  // Number of seconds left until timeout
  double SecsLeft() const { return tmlim - Elapsed(); }
  // Whether it timed out
  bool TimedOut() const { return Elapsed() >= tmlim; }

private:
  clock::time_point tpstart;
  double tmlim;
};

/*
 * Run a system command
 */
inline std::string exec(const char* cmd) {
  // https://stackoverflow.com/a/478960
  std::string result;
  std::array<char, 128> buffer;
#ifdef _WIN32
  std::shared_ptr<FILE> pipe(_popen(cmd, "r"), _pclose);
#else
  std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
#endif
  if (!pipe) throw std::runtime_error("popen() failed!");
  while (!feof(pipe.get())) {
    if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
      result += buffer.data();
  }
  return result;
}

/*
 * Simple tabu list
 */
struct TabuList {
  TabuList(int size, int tenure)
      : tabu(size, -tenure), iter(1), ten(tenure), sz(size) {}
  bool IsTabu(int u) const { return not tabu.empty() and tabu[u] + ten > iter; }
  void Add(int u) { tabu[u] = iter; }
  void Remove(int u) { tabu[u] = -ten; }
  void AdvanceIter() { ++iter; }
  int Since(int u) const { return IsTabu(u) ? tabu[u] : INT_MAX; }
  void Reset() { tabu.assign(sz, -ten), iter = 1; }
  VI tabu;
  int iter, ten, sz;
};

/*
 * Miscellaneous helper functions.
 */

// Check if a value v is in the range [lo,hi]
template <typename V> inline bool InRange(V v, V lo, V hi) {
  return v >= lo and v <= hi;
}

// Linear test for containment in a STL container. O(size(container))
inline bool LinearIn(const auto& container, const auto& v) {
  return std::find(std::begin(container), std::end(container), v) !=
         std::end(container);
}

// Fill a container with a value
inline void Fill(const auto& container, const auto& value) {
  std::fill(std::begin(container), std::end(container), value);
}

// Get an iota
inline VI Iota(size_t size, int start = 0) {
  VI v(size);
  std::iota(std::begin(v), std::end(v), start);
  return v;
}

// The Fractional part of a value
inline double Fractional(double val) {
  return std::fabs(val - std::round(val));
}

// Helper to save exit code when calling exit()
namespace util {
inline int lastExitCode = EXIT_SUCCESS;
}
#define exit(x)                                                                \
  (exit)(util::lastExitCode =                                                  \
             (x)) // hack, see https://stackoverflow.com/a/2196940

// Option for verbosity levels
namespace opt {
struct options_counter {
  int count = 0;
};
inline options_counter verbosec; // Verbosity level
inline void validate(boost::any& v, std::vector<std::string> const&,
                     options_counter*, long) {
  if (v.empty())
    v = options_counter{1};
  else
    ++boost::any_cast<options_counter&>(v).count;
}
} // namespace opt

/*
 * Easy printing (w/ verbosec).
 * todo move somewhere else, so this file doesn't depend on `options.h`
 */
template <typename... Ts> void pr(int l, Ts... v) {
  if (opt::verbosec.count >= l) {
    if (l > 0) fmt::print("{}", std::string(size_t(2 * l), ' '));
    fmt::print(v...);
  }
}
template <typename... Ts> void pr(Ts... v) { pr(-1, v...); }
template <typename... Ts> void prt(int l, Timer t, Ts... v) {
  if (opt::verbosec.count >= l) {
    if (l > 0) fmt::print("{}", std::string(size_t(2 * l), ' '));
    fmt::print("[{:.2f}] ", t.Elapsed());
    fmt::print(v...);
  }
}
template <typename... Ts> void prt(Timer t, Ts... v) { prt(-1, t, v...); }

/*
 * Wrapper for boost::multi_array
 */
template <typename T> struct MulArr2 : public boost::multi_array<T, 2> {
  MulArr2() = default;
  MulArr2(int d1, int d2) : boost::multi_array<T, 2>(boost::extents[d1][d2]) {}
  MulArr2(int d1, int d2, T value) : MulArr2(d1, d2) { fill(value); }
  inline void resize(int d1, int d2) {
    boost::multi_array<T, 2>::template resize(boost::extents[d1][d2]);
  }
  inline void assign(int d1, int d2, T value) { resize(d1, d2), fill(value); }
  inline void fill(T value) {
    std::fill_n(this->data(), this->num_elements(), value);
  }
};
using VI2 = MulArr2<int>;
using VD2 = MulArr2<double>;
using VB2 = MulArr2<bool>;