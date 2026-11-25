// Repository: Library3372
// File: benchmarks/math/mod_mul/generators/random64.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/mod_mul/generators/random64.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cstdlib>
#include <iostream>
#include <random>

int main(int argc, char* argv[]) {
  if (argc < 3) return 1;
  int q = std::atoi(argv[1]);
  int k = std::atoi(argv[2]);

  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::mt19937_64 rng(0x8e9d7c6b5a493827ULL);
  std::uniform_int_distribution<unsigned long long> m_dist((1ULL << 62) + 1ULL,
                                                           (1ULL << 63) - 1ULL);

  std::cout << q << '\n';
  for (int i = 0; i < q; ++i) {
    unsigned long long m = m_dist(rng) | 1ULL;
    std::uniform_int_distribution<unsigned long long> xy_dist(1ULL, m - 1ULL);
    unsigned long long x = xy_dist(rng);
    unsigned long long y = xy_dist(rng);
    std::cout << m << ' ' << x << ' ' << y << ' ' << k << '\n';
  }
  return 0;
}
