// Repository: Library3372
// File: benchmarks/math/enumerate_primes/generators/value.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/enumerate_primes/generators/value.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cassert>
#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[]) {
  assert(argc >= 2);
  int n = std::atoi(argv[1]);

  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::cout << n << '\n';
  return 0;
}
