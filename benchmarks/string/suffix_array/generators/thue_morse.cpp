// Repository: Library3372
// File: benchmarks/string/suffix_array/generators/thue_morse.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/string/suffix_array/generators/thue_morse.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cstdlib>
#include <iostream>

int main(int argc, char** argv) {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  (void)argc;
  int n = std::atoi(argv[1]);

  std::cout << n << "\n";
  for (int i = 0; i < n; ++i) {
    if (i > 0) std::cout << ' ';
    std::cout << (__builtin_popcount((unsigned int)i) & 1);
  }
  std::cout << "\n";
  return 0;
}
