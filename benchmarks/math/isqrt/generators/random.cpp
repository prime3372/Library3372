// Repository: Library3372
// File: benchmarks/math/isqrt/generators/random.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/isqrt/generators/random.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <random>

int main(int argc, char* argv[]) {
  assert(argc >= 4);
  int q = std::atoi(argv[1]);
  long long min_val = std::atoll(argv[2]);
  long long max_val = std::atoll(argv[3]);

  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::mt19937_64 rng(0x6a09e667f3bcc908ULL);
  std::uniform_int_distribution<long long> dist(min_val, max_val);

  std::cout << q << '\n';
  for (int i = 0; i < q; ++i) {
    std::cout << dist(rng) << (i + 1 == q ? '\n' : ' ');
  }
  return 0;
}
