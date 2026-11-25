// Repository: Library3372
// File: benchmarks/data_structure/lazy_segment_tree/generators/random.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/data_structure/lazy_segment_tree/generators/random.cpp
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
  std::mt19937_64 rng(0x3c6ef372fe94f82bULL);
  std::uniform_int_distribution<int> val_dist(0, kMod - 1);
  std::uniform_int_distribution<int> pos_dist(0, n - 1);
  std::uniform_int_distribution<int> bound_dist(0, n);

  std::cout << n << ' ' << q << '\n';
  for (int i = 0; i < n; ++i) {
    std::cout << val_dist(rng) << (i + 1 == n ? '\n' : ' ');
  }

  for (int i = 0; i < q; ++i) {
    int r = (int)(rng() % 10);
    if (r < 4) {
      int ql = bound_dist(rng);
      int qr = bound_dist(rng);
      if (ql > qr) std::swap(ql, qr);
      std::cout << "0 " << ql << ' ' << qr << ' ' << val_dist(rng) << ' '
                << val_dist(rng) << '\n';
    } else if (r < 5) {
      std::cout << "1 " << pos_dist(rng) << ' ' << val_dist(rng) << '\n';
    } else if (r < 6) {
      std::cout << "2 " << pos_dist(rng) << '\n';
    } else {
      int ql = bound_dist(rng);
      int qr = bound_dist(rng);
      if (ql > qr) std::swap(ql, qr);
      std::cout << "3 " << ql << ' ' << qr << '\n';
    }
  }
  return 0;
}
