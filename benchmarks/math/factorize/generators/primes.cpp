// Repository: Library3372
// File: benchmarks/math/factorize/generators/primes.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/factorize/generators/primes.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cstdlib>
#include <iostream>
#include <random>
#include <vector>

#include "library3372/math/is_prime.hpp"

int main(int argc, char* argv[]) {
  if (argc < 4)
    ;
  int q = std::atoi(argv[1]);
  long long min_val = std::atoll(argv[2]);
  long long max_val = std::atoll(argv[3]);

  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::mt19937_64 rng(0x2468ace013579bdfULL);
  std::uniform_int_distribution<long long> dist(min_val, max_val);

  constexpr int pool_size = 4096;
  std::vector<long long> pool;
  pool.reserve(pool_size);
  while ((int)pool.size() < pool_size) {
    long long x = dist(rng) | 1LL;
    while (x <= max_val && !library3372::is_prime(x)) x += 2;
    if (x <= max_val) pool.push_back(x);
  }

  std::uniform_int_distribution<int> idx_dist(0, pool_size - 1);
  std::cout << q << '\n';
  for (int i = 0; i < q; ++i) {
    std::cout << pool[idx_dist(rng)] << (i + 1 == q ? '\n' : ' ');
  }
  return 0;
}
