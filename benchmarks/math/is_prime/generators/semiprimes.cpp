// Repository: Library3372
// File: benchmarks/math/is_prime/generators/semiprimes.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/is_prime/generators/semiprimes.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cstdlib>
#include <iostream>
#include <random>
#include <vector>

#include "library3372/math/is_prime.hpp"

int main(int argc, char* argv[]) {
  std::ios::sync_with_stdio(false);

  int q = std::atoi(argv[1]);
  long long p_min = std::atoll(argv[2]);
  long long p_max = std::atoll(argv[3]);

  std::mt19937_64 rng(0x369cf02468ace135ULL);
  std::uniform_int_distribution<long long> dist(p_min, p_max);

  constexpr int pool_size = 2048;
  std::vector<long long> pool;
  pool.reserve(pool_size);
  while ((int)pool.size() < pool_size) {
    long long x = dist(rng) | 1LL;
    while (x <= p_max && !library3372::is_prime(x)) x += 2;
    if (x <= p_max) pool.push_back(x);
  }

  std::uniform_int_distribution<int> idx_dist(0, pool_size - 1);
  std::cout << q << '\n';
  for (int i = 0; i < q; ++i) {
    long long p = pool[idx_dist(rng)];
    long long r = pool[idx_dist(rng)];
    std::cout << p * r << (i + 1 == q ? '\n' : ' ');
  }
  return 0;
}
