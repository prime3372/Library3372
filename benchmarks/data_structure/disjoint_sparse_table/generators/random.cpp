// Repository: Library3372
// File: benchmarks/data_structure/disjoint_sparse_table/generators/random.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/data_structure/disjoint_sparse_table/generators/random.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <random>
#include <utility>

int main(int argc, char* argv[]) {
  assert(argc >= 3);
  int n = std::atoi(argv[1]);
  int q = std::atoi(argv[2]);
  assert(n >= 1 && q >= 1);

  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  constexpr int kMod = 998244353;
  std::mt19937_64 rng(0xbb67ae8584caa73bULL);
  std::uniform_int_distribution<int> val_dist(0, kMod - 1);
  std::uniform_int_distribution<int> bound_dist(0, n);

  std::cout << n << ' ' << q << '\n';
  for (int i = 0; i < n; ++i) {
    std::cout << val_dist(rng) << ' ' << val_dist(rng) << '\n';
  }
  for (int i = 0; i < q; ++i) {
    int l = bound_dist(rng);
    int r = bound_dist(rng);
    if (l > r) std::swap(l, r);
    std::cout << l << ' ' << r << '\n';
  }
  return 0;
}
