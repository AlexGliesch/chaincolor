#pragma once
#include <climits>
#include <cstdint>
#include <ctime>
#include <random>
#include <unistd.h>

/*
 * Helper structure for random number generation.
 */
struct Random {
  // Init with seed. If seed=0, generates an unique random seed and returns it.
  static void SetSeed(uint64_t new_seed) {
    seed = new_seed == 0 ? UniqueRandomSeed() : new_seed;
    rng.seed(seed);
  }

  // Returns a random integer in range [from, to], inclusive
  static inline int RandInt(int from = 0, int to = INT_MAX) {
    static std::uniform_int_distribution<int> d;
    return d(rng, decltype(d)::param_type{from, to});
  }

  // Returns a random double in the range [from, to], inclusive
  static inline double RandDouble(double from, double to) {
    static std::uniform_real_distribution<double> d;
    return d(rng, decltype(d)::param_type{from, to});
  }

  // Returns a random bool
  static inline bool RandBool() { return (bool)RandInt(0, 1); }

  // Select 'k' elements uniformly at random from 'v' and put them in 'result'. This
  // resizes 'result' to be of size 'k'. Complexity: O(k)
  template <typename T>
  static inline void Choice(const std::vector<T>& v, std::vector<T>& result, int k) {
    int n = v.size();
    result.resize(k);
    for (int i = 0; i < k; ++i) result[i] = v[i];
    for (int i = k + 1; i < n; ++i) {
      int j = RandInt(0, i);
      if (j < k) result[j] = v[i];
    }
  }

  // Class to handle reservoir sampling
  struct ReservoirSampling {
    bool Consider() { return (1.0 / double(++num)) >= RandDouble(0.0, 1.0); }
    void Reset(int num = 0) { this->num = num; }
    int num = 0;
  };

  // Random number generator
  static inline std::mt19937 rng;

  // Initial seed to rng
  static inline uint64_t seed;

private:
  // Returns a unique random seed which is based on calls to clock(), time() and getpid()
  // TODO add source
  static inline uint64_t UniqueRandomSeed() {
    uint64_t a = (uint64_t)clock(), b = (uint64_t)time(nullptr), c = (uint64_t)getpid();
    a = (a - b - c) ^ (c >> 13);
    b = (b - c - a) ^ (a << 8);
    c = (c - a - b) ^ (b >> 13);
    a = (a - b - c) ^ (c >> 12);
    b = (b - c - a) ^ (a << 16);
    c = (c - a - b) ^ (b >> 5);
    a = (a - b - c) ^ (c >> 3);
    b = (b - c - a) ^ (a << 10);
    c = (c - a - b) ^ (b >> 15);
    return c;
  }
};