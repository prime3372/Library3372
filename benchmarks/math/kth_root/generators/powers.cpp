// Repository: Library3372
// File: benchmarks/math/kth_root/generators/powers.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/kth_root/generators/powers.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <bit>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <random>

int main(int argc, char* argv[]) {
  assert(argc >= 4);
  int q = std::atoi(argv[1]);
  int min_k = std::atoi(argv[2]);
  int max_k = std::atoi(argv[3]);

  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::mt19937_64 rng(0xa54ff53a5f1d36f1ULL);
  std::uniform_int_distribution<int> k_dist(min_k, max_k);
  std::uniform_int_distribution<int> offset_dist(-1, 1);

  std::cout << q << '\n';
  for (int i = 0; i < q; ++i) {
    int k = k_dist(rng);
    long long max_base = 1;
    while (true) {
      unsigned __int128 p = 1;
      bool ok = true;
      long long nb = max_base << 1;
      for (int j = 0; j < k; ++j) {
        p *= (unsigned __int128)nb;
        if (p > (unsigned __int128)std::numeric_limits<long long>::max()) {
          ok = false;
          break;
        }
      }
      if (!ok) break;
      max_base = nb;
    }
    std::uniform_int_distribution<long long> base_dist(1, max_base);
    long long m = base_dist(rng);
    unsigned __int128 p = 1;
    for (int j = 0; j < k; ++j) p *= (unsigned __int128)m;
    long long x = (long long)p + offset_dist(rng);
    if (x < 0) x = 0;
    std::cout << x << ' ' << k << '\n';
  }
  return 0;
}
