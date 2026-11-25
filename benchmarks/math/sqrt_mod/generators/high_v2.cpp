// Repository: Library3372
// File: benchmarks/math/sqrt_mod/generators/high_v2.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/sqrt_mod/generators/high_v2.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <random>
#include <vector>

#include "library3372/math/is_prime.hpp"

int main(int argc, char* argv[]) {
  assert(argc >= 3);
  int q = std::atoi(argv[1]);
  int bits = std::atoi(argv[2]);
  assert(bits == 32 || bits == 64);

  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::vector<long long> pool;
  if (bits == 32) {
    for (int s = 22; s <= 28; ++s) {
      long long max_k = ((1LL << 31) - 2) >> s;
      for (long long k = 1; k <= max_k; k += 2) {
        long long p = (k << s) + 1;
        if (p >= 100000000LL && library3372::is_prime(p)) pool.push_back(p);
      }
    }
  } else {
    for (int s = 50; s <= 58; ++s) {
      long long max_k = (9223372036854775807LL - 1) >> s;
      for (long long k = 1; k <= max_k; k += 2) {
        long long p = (k << s) + 1;
        if (p >= (1LL << 60) && library3372::is_prime(p)) pool.push_back(p);
      }
    }
  }
  assert(!pool.empty());

  std::mt19937_64 rng(0x4b8e2f0a19c3d577ULL);
  std::uniform_int_distribution<int> idx_dist(0, (int)pool.size() - 1);

  std::cout << q << '\n';
  for (int i = 0; i < q; ++i) {
    long long p = pool[idx_dist(rng)];
    std::uniform_int_distribution<long long> x_dist(0, p - 1);
    long long x = x_dist(rng);
    unsigned __int128 sq = (unsigned __int128)x * (unsigned __int128)x;
    long long a = (long long)(sq % (unsigned long long)p);
    std::cout << a << ' ' << p << '\n';
  }
  return 0;
}
