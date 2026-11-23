// Repository: Library3372
// File: benchmarks/math/convolution_mod_2_64/generators/random.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/convolution_mod_2_64/generators/random.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cstdlib>
#include <iostream>
#include <random>

int main(int argc, char* argv[]) {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  if (argc < 3) return 1;
  int n = std::atoi(argv[1]);
  int m = std::atoi(argv[2]);

  std::mt19937_64 rng(0x91e10da5c79e7b1dULL);

  std::cout << n << ' ' << m << '\n';
  for (int i = 0; i < n; ++i) {
    if (i > 0) std::cout << ' ';
    std::cout << rng();
  }
  std::cout << '\n';
  for (int i = 0; i < m; ++i) {
    if (i > 0) std::cout << ' ';
    std::cout << rng();
  }
  std::cout << '\n';
  return 0;
}
