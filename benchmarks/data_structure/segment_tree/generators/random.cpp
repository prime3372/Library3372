// Repository: Library3372
// File: benchmarks/data_structure/segment_tree/generators/random.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/data_structure/segment_tree/generators/random.cpp
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

  constexpr int kMaxVal = 1000000000;
  std::mt19937_64 rng(0x517cc1b727220a95ULL);
  std::uniform_int_distribution<int> val_dist(0, kMaxVal);
  std::uniform_int_distribution<int> pos_dist(0, n - 1);
  std::uniform_int_distribution<int> bound_dist(0, n);

  std::cout << n << ' ' << q << '\n';
  for (int i = 0; i < n; ++i) {
    std::cout << val_dist(rng) << (i + 1 == n ? '\n' : ' ');
  }

  for (int i = 0; i < q; ++i) {
    int type;
    if (mode == 0) {
      int r = (int)(rng() % 10);
      if (r < 3) {
        type = 0;
      } else if (r < 4) {
        type = 1;
      } else if (r < 8) {
        type = 2;
      } else {
        type = (r == 8) ? 3 : 4;
      }
    } else {
      int r = (int)(rng() % 5);
      if (r == 0) {
        type = 0;
      } else if (r < 3) {
        type = 3;
      } else {
        type = 4;
      }
    }

    if (type == 0) {
      std::cout << "0 " << pos_dist(rng) << ' ' << val_dist(rng) << '\n';
    } else if (type == 1) {
      std::cout << "1 " << pos_dist(rng) << '\n';
    } else if (type == 2) {
      int l = bound_dist(rng);
      int r = bound_dist(rng);
      if (l > r) std::swap(l, r);
      std::cout << "2 " << l << ' ' << r << '\n';
    } else if (type == 3) {
      std::cout << "3 " << bound_dist(rng) << ' ' << val_dist(rng) << '\n';
    } else {
      std::cout << "4 " << bound_dist(rng) << ' ' << val_dist(rng) << '\n';
    }
  }
  return 0;
}
