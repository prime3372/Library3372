// Repository: Library3372
// File: benchmarks/string/suffix_array/generators/periodic.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/string/suffix_array/generators/periodic.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cstdlib>
#include <iostream>

int main(int argc, char** argv) {
  if (argc < 3) return 1;
  int n = std::atoi(argv[1]);
  int period = std::atoi(argv[2]);

  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::cout << n << "\n";
  for (int i = 0; i < n; ++i) {
    if (i > 0) std::cout << ' ';
    std::cout << (i % period);
  }
  std::cout << "\n";
  return 0;
}
