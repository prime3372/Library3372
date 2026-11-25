// Repository: Library3372
// File: benchmarks/math/mod_mul/generators/random32.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/mod_mul/generators/random32.cpp
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

  std::mt19937_64 rng(0x517cc1b727220a95ULL);
  std::uniform_int_distribution<unsigned int> m_dist(3U, 0xffffffffU);

  std::cout << q << '\n';
  for (int i = 0; i < q; ++i) {
    unsigned int m = m_dist(rng) | 1U;
    std::uniform_int_distribution<unsigned int> xy_dist(1U, m - 1U);
    unsigned int x = xy_dist(rng);
    unsigned int y = xy_dist(rng);
    std::cout << m << ' ' << x << ' ' << y << ' ' << k << '\n';
  }
  return 0;
}
