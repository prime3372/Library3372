// Repository: Library3372
// File: benchmarks/string/suffix_array/generators/random.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/string/suffix_array/generators/random.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cstdlib>
#include <iostream>
#include <random>

int main(int argc, char** argv) {
  if (argc < 3) return 1;
  int n = std::atoi(argv[1]);
  int sigma = std::atoi(argv[2]);

  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::mt19937_64 rng(0x9e3779b97f4a7c15ULL ^ ((unsigned long long)n << 32) ^
                      (unsigned long long)sigma);
  std::uniform_int_distribution<int> dist(0, sigma - 1);

  std::cout << n << "\n";
  for (int i = 0; i < n; ++i) {
    if (i > 0) std::cout << ' ';
    std::cout << dist(rng);
  }
  std::cout << "\n";
  return 0;
}
