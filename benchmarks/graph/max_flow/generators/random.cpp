// Repository: Library3372
// File: benchmarks/graph/max_flow/generators/random.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/graph/max_flow/generators/random.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <random>

int main(int argc, char* argv[]) {
  assert(argc >= 4);
  int n = std::atoi(argv[1]);
  int m = std::atoi(argv[2]);
  long long max_cap = std::atoll(argv[3]);
  assert(n >= 2 && m >= 1 && max_cap >= 1);

  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::mt19937_64 rng(0x243f6a8885a308d3ULL);
  std::uniform_int_distribution<int> vdist(0, n - 1);
  std::uniform_int_distribution<long long> cdist(1, max_cap);

  std::cout << n << ' ' << m << " 0 " << (n - 1) << '\n';
  for (int i = 0; i < m; ++i) {
    int u = vdist(rng);
    int v = vdist(rng);
    long long c = cdist(rng);
    std::cout << u << ' ' << v << ' ' << c << '\n';
  }
  return 0;
}
