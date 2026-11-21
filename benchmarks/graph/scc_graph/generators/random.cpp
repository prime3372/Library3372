// Repository: Library3372
// File: benchmarks/graph/scc_graph/generators/random.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/graph/scc_graph/generators/random.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cstdlib>
#include <iostream>
#include <random>

int main(int argc, char** argv) {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n = std::atoi(argv[1]);
  int m = std::atoi(argv[2]);

  std::mt19937_64 rng(0x243f6a8885a308d3ULL ^ ((unsigned long long)n << 32) ^
                      (unsigned long long)m);
  std::uniform_int_distribution<int> vdist(0, n - 1);

  std::cout << n << " " << m << "\n";
  for (int i = 0; i < m; ++i) {
    std::cout << vdist(rng) << " " << vdist(rng) << "\n";
  }
  return 0;
}
