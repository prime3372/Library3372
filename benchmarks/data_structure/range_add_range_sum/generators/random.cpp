// Repository: Library3372
// File: benchmarks/data_structure/range_add_range_sum/generators/random.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/data_structure/range_add_range_sum/generators/random.cpp
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
  assert(n >= 1 && q >= 1 && mode >= 0 && mode <= 2);

  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::mt19937_64 rng(0x243f6a8885a308d3ULL);
  std::uniform_int_distribution<long long> signed_dist(-1000000, 1000000);
  std::uniform_int_distribution<long long> nonneg_dist(0, 1000000);
  std::uniform_int_distribution<int> bound_dist(0, n);
  std::uniform_int_distribution<int> op_dist(0, 99);

  std::cout << n << ' ' << q << '\n';
  long long total = 0;
  for (int i = 0; i < n; ++i) {
    long long v = (mode == 1) ? signed_dist(rng) : nonneg_dist(rng);
    total += v;
    std::cout << v << (i + 1 == n ? '\n' : ' ');
  }

  for (int i = 0; i < q; ++i) {
    int type = 0;
    if (mode == 0) {
      int p = op_dist(rng);
      if (p < 35) {
        type = 0;
      } else if (p < 70) {
        type = 1;
      } else if (p < 85) {
        type = 2;
      } else {
        type = 3;
      }
    } else if (mode == 1) {
      type = (int)(rng() & 1);
    } else {
      int p = op_dist(rng);
      if (p < 20) {
        type = 0;
      } else if (p < 60) {
        type = 2;
      } else {
        type = 3;
      }
    }

    if (type <= 1) {
      int l, r;
      if (mode == 1) {
        int len = (int)(rng() % (unsigned int)std::min(16, n)) + 1;
        l = (int)(rng() % (unsigned int)(n - len + 1));
        r = l + len;
      } else {
        l = bound_dist(rng);
        r = bound_dist(rng);
        if (l > r) std::swap(l, r);
      }
      if (type == 0) {
        long long x = (mode == 1) ? signed_dist(rng) : nonneg_dist(rng);
        total += (long long)(r - l) * x;
        std::cout << "0 " << l << ' ' << r << ' ' << x << '\n';
      } else {
        std::cout << "1 " << l << ' ' << r << '\n';
      }
    } else if (type == 2) {
      int l = bound_dist(rng);
      long long max_x = (long long)((__int128)total * (n - l) / n) + 1000;
      std::uniform_int_distribution<long long> lim_dist(0, max_x);
      std::cout << "2 " << l << ' ' << lim_dist(rng) << '\n';
    } else {
      int r = bound_dist(rng);
      long long max_x = (long long)((__int128)total * r / n) + 1000;
      std::uniform_int_distribution<long long> lim_dist(0, max_x);
      std::cout << "3 " << r << ' ' << lim_dist(rng) << '\n';
    }
  }
  return 0;
}
