// Repository: Library3372
// File: benchmarks/math/sqrt_mod/generators/random.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/sqrt_mod/generators/random.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <random>
#include <vector>

#include "library3372/math/is_prime.hpp"

int main(int argc, char* argv[]) {
  assert(argc >= 5);
  int q = std::atoi(argv[1]);
  long long min_p = std::atoll(argv[2]);
  long long max_p = std::atoll(argv[3]);
  int qr_only = std::atoi(argv[4]);

  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::mt19937_64 rng(0x7c9d1b3f5a2e8041ULL);
  std::uniform_int_distribution<long long> p_dist(min_p, max_p);

  constexpr int pool_size = 4096;
  std::vector<long long> pool;
  pool.reserve(pool_size);
  while ((int)pool.size() < pool_size) {
    long long p = p_dist(rng);
    if (p > 2) p |= 1LL;
    while (p <= max_p && !library3372::is_prime(p)) {
      p += (p == 2 ? 1 : 2);
    }
    if (p <= max_p) pool.push_back(p);
  }

  std::uniform_int_distribution<int> idx_dist(0, pool_size - 1);
  std::cout << q << '\n';
  for (int i = 0; i < q; ++i) {
    long long p = pool[idx_dist(rng)];
    std::uniform_int_distribution<long long> a_dist(0, p - 1);
    long long a = a_dist(rng);
    if (qr_only) {
      unsigned __int128 sq = (unsigned __int128)a * (unsigned __int128)a;
      a = (long long)(sq % (unsigned long long)p);
    }
    std::cout << a << ' ' << p << '\n';
  }
  return 0;
}
