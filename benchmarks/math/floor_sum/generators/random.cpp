// Repository: Library3372
// File: benchmarks/math/floor_sum/generators/random.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/floor_sum/generators/random.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <random>

int main(int argc, char* argv[]) {
  assert(argc >= 6);
  int q = std::atoi(argv[1]);
  long long max_n = std::atoll(argv[2]);
  long long max_m = std::atoll(argv[3]);
  long long min_ab = std::atoll(argv[4]);
  long long max_ab = std::atoll(argv[5]);
  assert(q >= 1 && max_n >= 0 && max_m >= 1 && min_ab <= max_ab);

  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::mt19937_64 rng(0x517cc1b727220a95ULL);
  std::uniform_int_distribution<long long> dist_n(0, max_n);
  std::uniform_int_distribution<long long> dist_m(1, max_m);
  std::uniform_int_distribution<long long> dist_ab(min_ab, max_ab);

  std::cout << q << '\n';
  for (int i = 0; i < q; ++i) {
    std::cout << dist_n(rng) << ' ' << dist_m(rng) << ' ' << dist_ab(rng) << ' '
              << dist_ab(rng) << '\n';
  }
  return 0;
}
