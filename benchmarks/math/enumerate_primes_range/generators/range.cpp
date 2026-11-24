// Repository: Library3372
// File: benchmarks/math/enumerate_primes_range/generators/range.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/enumerate_primes_range/generators/range.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cassert>
#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[]) {
  assert(argc >= 3);
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  long long l = std::atoll(argv[1]);
  long long r = std::atoll(argv[2]);
  std::cout << l << ' ' << r << '\n';
  return 0;
}
