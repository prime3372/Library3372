// Repository: Library3372
// File: benchmarks/data_structure/union_find/generators/random.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/data_structure/union_find/generators/random.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <random>

int main(int argc, char* argv[]) {
  assert(argc >= 3);
  int n = std::atoi(argv[1]);
  int q = std::atoi(argv[2]);
  assert(n >= 1 && q >= 1);

  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::mt19937_64 rng(0x243f6a8885a308d3ULL);
  std::uniform_int_distribution<int> vdist(0, n - 1);
  std::uniform_int_distribution<int> opdist(0, 99);

  std::cout << n << ' ' << q << '\n';
  for (int i = 0; i < q; ++i) {
    int p = opdist(rng);
    if (p < 40) {
      std::cout << "0 " << vdist(rng) << ' ' << vdist(rng) << '\n';
    } else if (p < 65) {
      std::cout << "1 " << vdist(rng) << ' ' << vdist(rng) << '\n';
    } else if (p < 80) {
      std::cout << "2 " << vdist(rng) << '\n';
    } else if (p < 95) {
      std::cout << "3 " << vdist(rng) << '\n';
    } else {
      std::cout << "4\n";
    }
  }
  return 0;
}
