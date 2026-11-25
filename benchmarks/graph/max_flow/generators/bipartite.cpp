// Repository: Library3372
// File: benchmarks/graph/max_flow/generators/bipartite.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/graph/max_flow/generators/bipartite.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <random>

int main(int argc, char* argv[]) {
  assert(argc >= 4);
  int l = std::atoi(argv[1]);
  int r = std::atoi(argv[2]);
  int m = std::atoi(argv[3]);
  assert(l >= 1 && r >= 1 && m >= 1);

  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n = l + r + 2;
  int s = 0;
  int t = n - 1;
  int total_m = l + r + m;

  std::mt19937_64 rng(0x13198a2e03707344ULL);
  std::uniform_int_distribution<int> ldist(1, l);
  std::uniform_int_distribution<int> rdist(l + 1, l + r);

  std::cout << n << ' ' << total_m << ' ' << s << ' ' << t << '\n';
  for (int i = 1; i <= l; ++i) std::cout << s << ' ' << i << " 1\n";
  for (int i = l + 1; i <= l + r; ++i) std::cout << i << ' ' << t << " 1\n";
  for (int i = 0; i < m; ++i) {
    std::cout << ldist(rng) << ' ' << rdist(rng) << " 1\n";
  }
  return 0;
}
