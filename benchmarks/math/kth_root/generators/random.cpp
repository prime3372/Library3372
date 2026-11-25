// Repository: Library3372
// File: benchmarks/math/kth_root/generators/random.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/kth_root/generators/random.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <random>

int main(int argc, char* argv[]) {
  assert(argc >= 6);
  int q = std::atoi(argv[1]);
  long long min_a = std::atoll(argv[2]);
  long long max_a = std::atoll(argv[3]);
  long long min_k = std::atoll(argv[4]);
  long long max_k = std::atoll(argv[5]);

  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::mt19937_64 rng(0x517cc1b727220a95ULL);
  std::uniform_int_distribution<long long> dist_a(min_a, max_a);
  std::uniform_int_distribution<long long> dist_k(min_k, max_k);

  std::cout << q << '\n';
  for (int i = 0; i < q; ++i) {
    std::cout << dist_a(rng) << ' ' << dist_k(rng) << '\n';
  }
  return 0;
}
