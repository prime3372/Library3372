// Repository: Library3372
// File: benchmarks/graph/max_flow/generators/grid.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/graph/max_flow/generators/grid.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <random>

int main(int argc, char* argv[]) {
  assert(argc >= 4);
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int h = std::atoi(argv[1]);
  int w = std::atoi(argv[2]);
  long long max_cap = std::atoll(argv[3]);
  assert(h >= 2 && w >= 2 && max_cap >= 1);

  int n = h * w;
  int m = h * (w - 1) + (h - 1) * w;

  std::mt19937_64 rng(0xa4093822299f31d0ULL);
  std::uniform_int_distribution<long long> cdist(1, max_cap);

  std::cout << n << ' ' << m << " 0 " << (n - 1) << '\n';
  for (int r = 0; r < h; ++r) {
    for (int c = 0; c < w; ++c) {
      int u = r * w + c;
      if (c + 1 < w) {
        std::cout << u << ' ' << (u + 1) << ' ' << cdist(rng) << '\n';
      }
      if (r + 1 < h) {
        std::cout << u << ' ' << (u + w) << ' ' << cdist(rng) << '\n';
      }
    }
  }
  return 0;
}
