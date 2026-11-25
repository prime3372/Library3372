// Repository: Library3372
// File: benchmarks/math/static_modint_raw/generators/random.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/static_modint_raw/generators/random.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <random>

int main(int argc, char* argv[]) {
  assert(argc >= 3);
  int n = std::atoi(argv[1]);
  int k = std::atoi(argv[2]);

  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::mt19937_64 rng(0x9e3779b97f4a7c15ULL);
  std::uniform_int_distribution<int> dist(0, 998244352);

  std::cout << n << ' ' << k << '\n';
  for (int i = 0; i < n; ++i) {
    if (i > 0) std::cout << ' ';
    std::cout << dist(rng);
  }
  std::cout << '\n';
  return 0;
}
