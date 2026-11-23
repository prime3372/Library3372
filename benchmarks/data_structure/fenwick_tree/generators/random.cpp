// Repository: Library3372
// File: benchmarks/data_structure/fenwick_tree/generators/random.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/data_structure/fenwick_tree/generators/random.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <random>
#include <vector>

int main(int argc, char* argv[]) {
  assert(argc >= 4);
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n = std::atoi(argv[1]);
  int q = std::atoi(argv[2]);
  int mode = std::atoi(argv[3]);
  assert(n >= 1 && q >= 1);

  std::mt19937_64 rng(0x6a09e667f3bcc908ULL);
  std::uniform_int_distribution<long long> val_dist(0, 1000000);
  std::uniform_int_distribution<int> pos_dist(0, n - 1);
  std::uniform_int_distribution<int> bound_dist(0, n);
  std::uniform_int_distribution<int> op_dist(0, 99);

  std::vector<long long> a(n);
  long long total = 0;
  for (int i = 0; i < n; ++i) {
    a[i] = val_dist(rng);
    total += a[i];
  }

  std::cout << n << ' ' << q << '\n';
  for (int i = 0; i < n; ++i) {
    if (i > 0) std::cout << ' ';
    std::cout << a[i];
  }
  std::cout << '\n';

  for (int i = 0; i < q; ++i) {
    int p = op_dist(rng);
    int type = 0;
    if (mode == 0) {
      if (p < 20) {
        type = 0;
      } else if (p < 35) {
        type = 1;
      } else if (p < 50) {
        type = 2;
      } else if (p < 75) {
        type = 3;
      } else if (p < 88) {
        type = 4;
      } else {
        type = 5;
      }
    } else {
      if (p < 15) {
        type = 0;
      } else if (p < 25) {
        type = 1;
      } else if (p < 62) {
        type = 4;
      } else {
        type = 5;
      }
    }

    if (type == 0) {
      int idx = pos_dist(rng);
      long long x = val_dist(rng);
      a[idx] += x;
      total += x;
      std::cout << "0 " << idx << ' ' << x << '\n';
    } else if (type == 1) {
      int idx = pos_dist(rng);
      long long x = val_dist(rng);
      total += x - a[idx];
      a[idx] = x;
      std::cout << "1 " << idx << ' ' << x << '\n';
    } else if (type == 2) {
      std::cout << "2 " << pos_dist(rng) << '\n';
    } else if (type == 3) {
      int l = bound_dist(rng);
      int r = bound_dist(rng);
      if (l > r) std::swap(l, r);
      std::cout << "3 " << l << ' ' << r << '\n';
    } else if (type == 4) {
      int l = bound_dist(rng);
      std::uniform_int_distribution<long long> lim_dist(0, total + 100);
      std::cout << "4 " << l << ' ' << lim_dist(rng) << '\n';
    } else {
      int r = bound_dist(rng);
      std::uniform_int_distribution<long long> lim_dist(0, total + 100);
      std::cout << "5 " << r << ' ' << lim_dist(rng) << '\n';
    }
  }
  return 0;
}
