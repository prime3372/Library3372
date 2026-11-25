// Repository: Library3372
// File: benchmarks/string/suffix_array/generators/fibonacci.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/string/suffix_array/generators/fibonacci.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>

int main(int argc, char** argv) {
  if (argc < 2) return 1;
  int n = std::atoi(argv[1]);

  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::string a = "0";
  std::string b = "1";
  while ((int)b.size() < n) {
    std::string c = b + a;
    a = std::move(b);
    b = std::move(c);
  }

  std::cout << n << "\n";
  for (int i = 0; i < n; ++i) {
    if (i > 0) std::cout << ' ';
    std::cout << b[i];
  }
  std::cout << "\n";
  return 0;
}
