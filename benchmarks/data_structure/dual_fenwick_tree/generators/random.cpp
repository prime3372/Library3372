// Repository: Library3372
// File: benchmarks/data_structure/dual_fenwick_tree/generators/random.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/data_structure/dual_fenwick_tree/generators/random.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <random>
#include <utility>

int main(int argc, char* argv[]) {
  assert(argc >= 4);
  int n = std::atoi(argv[1]);
  int q = std::atoi(argv[2]);
  int mode = std::atoi(argv[3]);
  assert(n >= 1 && q >= 1 && (mode == 0 || mode == 1));

  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::mt19937_64 rng(0x9b05688c2b3e6c1fULL);
  std::uniform_int_distribution<long long> val_dist(-1000000000, 1000000000);
  std::uniform_int_distribution<int> pos_dist(0, n - 1);
  std::uniform_int_distribution<int> bound_dist(0, n);

  std::cout << n << ' ' << q << '\n';
  for (int i = 0; i < n; ++i) {
    std::cout << val_dist(rng) << (i + 1 == n ? '\n' : ' ');
  }

  for (int i = 0; i < q; ++i) {
    int r = (int)(rng() % 10);
    if (r < 4) {
      int ql, qr;
      if (mode == 0) {
        ql = bound_dist(rng);
        qr = bound_dist(rng);
        if (ql > qr) std::swap(ql, qr);
      } else {
        int len = (int)(rng() % (unsigned int)std::min(16, n)) + 1;
        ql = (int)(rng() % (unsigned int)(n - len + 1));
        qr = ql + len;
      }
      std::cout << "0 " << ql << ' ' << qr << ' ' << val_dist(rng) << '\n';
    } else if (r < 5) {
      std::cout << "1 " << pos_dist(rng) << ' ' << val_dist(rng) << '\n';
    } else {
      std::cout << "2 " << pos_dist(rng) << '\n';
    }
  }
  return 0;
}
