// Repository: Library3372
// File: benchmarks/graph/two_sat/generators/chain.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/graph/two_sat/generators/chain.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cassert>
#include <cstdlib>
#include <iostream>

int main(int argc, char** argv) {
  assert(argc >= 2);
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n = std::atoi(argv[1]);
  int m = 2 * (n - 1) + 1;

  std::cout << n << " " << m << "\n";
  for (int i = 0; i + 1 < n; ++i) {
    std::cout << i << " 0 " << (i + 1) << " 1\n";
    std::cout << i << " 1 " << (i + 1) << " 0\n";
  }
  std::cout << "0 1 " << (n - 1) << " 1\n";
  return 0;
}
