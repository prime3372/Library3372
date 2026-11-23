// Repository: Library3372
// File: benchmarks/graph/two_sat/generators/random.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/graph/two_sat/generators/random.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <random>

int main(int argc, char** argv) {
  assert(argc >= 3);
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n = std::atoi(argv[1]);
  int m = std::atoi(argv[2]);

  std::mt19937_64 rng(0x6a09e667f3bcc909ULL ^ ((unsigned long long)n << 32) ^
                      (unsigned long long)m);
  std::uniform_int_distribution<int> vdist(0, n - 1);
  std::uniform_int_distribution<int> bdist(0, 1);

  std::cout << n << " " << m << "\n";
  for (int k = 0; k < m; ++k) {
    int i = vdist(rng);
    int f = bdist(rng);
    int j = vdist(rng);
    int g = bdist(rng);
    std::cout << i << " " << f << " " << j << " " << g << "\n";
  }
  return 0;
}
