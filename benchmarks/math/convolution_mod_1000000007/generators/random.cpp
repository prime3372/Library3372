// Repository: Library3372
// File: benchmarks/math/convolution_mod_1000000007/generators/random.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/convolution_mod_1000000007/generators/random.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <random>

int main(int argc, char* argv[]) {
  assert(argc >= 3);
  int n = std::atoi(argv[1]);
  int m = std::atoi(argv[2]);

  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::mt19937_64 rng(0x517cc1b727220a95ULL);
  std::uniform_int_distribution<int> dist(0, 1000000006);

  std::cout << n << ' ' << m << '\n';
  for (int i = 0; i < n; ++i) {
    if (i > 0) std::cout << ' ';
    std::cout << dist(rng);
  }
  std::cout << '\n';
  for (int i = 0; i < m; ++i) {
    if (i > 0) std::cout << ' ';
    std::cout << dist(rng);
  }
  std::cout << '\n';
  return 0;
}
