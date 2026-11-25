// Repository: Library3372
// File: benchmarks/math/isqrt/generators/squares.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/isqrt/generators/squares.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cstdlib>
#include <iostream>
#include <random>

int main(int argc, char* argv[]) {
  if (argc < 4) return 1;
  int q = std::atoi(argv[1]);
  long long min_k = std::atoll(argv[2]);
  long long max_k = std::atoll(argv[3]);

  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::mt19937_64 rng(0xbb67ae8584caa73bULL);
  std::uniform_int_distribution<long long> k_dist(min_k, max_k);
  std::uniform_int_distribution<int> offset_dist(-1, 1);

  std::cout << q << '\n';
  for (int i = 0; i < q; ++i) {
    long long k = k_dist(rng);
    long long x = k * k + offset_dist(rng);
    if (x < 0) x = 0;
    std::cout << x << (i + 1 == q ? '\n' : ' ');
  }
  return 0;
}
