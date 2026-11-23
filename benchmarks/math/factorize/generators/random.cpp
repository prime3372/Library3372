// Repository: Library3372
// File: benchmarks/math/factorize/generators/random.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/factorize/generators/random.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <random>

int main(int argc, char* argv[]) {
  assert(argc >= 5);
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int q = std::atoi(argv[1]);
  long long min_val = std::atoll(argv[2]);
  long long max_val = std::atoll(argv[3]);
  int odd_only = std::atoi(argv[4]);

  std::mt19937_64 rng(0x13579bdf2468ace0ULL);
  std::uniform_int_distribution<long long> dist(min_val, max_val);

  std::cout << q << '\n';
  for (int i = 0; i < q; ++i) {
    long long x = dist(rng);
    if (odd_only) x |= 1LL;
    std::cout << x << (i + 1 == q ? '\n' : ' ');
  }
  return 0;
}
