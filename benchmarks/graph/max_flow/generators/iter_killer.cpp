// Repository: Library3372
// File: benchmarks/graph/max_flow/generators/iter_killer.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/graph/max_flow/generators/iter_killer.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cassert>
#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[]) {
  assert(argc >= 3);
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int d = std::atoi(argv[1]);
  int k = std::atoi(argv[2]);
  assert(d >= 1 && k >= d);

  int s = 0;
  int a_base = 1;
  int hub1 = a_base + k;
  int dead_base = hub1 + 1;
  int bypass = dead_base + d;
  int hub2 = bypass + 1;
  int b_base = hub2 + 1;
  int t = b_base + k;
  int n = t + 1;
  int m = 4 * k + 2 * d + 2;

  std::cout << n << ' ' << m << ' ' << s << ' ' << t << '\n';
  for (int i = 0; i < k; ++i) {
    std::cout << s << ' ' << (a_base + i) << " 1\n";
    std::cout << (a_base + i) << ' ' << hub1 << " 1\n";
  }
  for (int j = 0; j < d; ++j) {
    std::cout << hub1 << ' ' << (dead_base + j) << " 1\n";
    std::cout << (dead_base + j) << ' ' << hub2 << ' ' << k << '\n';
  }
  std::cout << hub1 << ' ' << bypass << ' ' << k << '\n';
  std::cout << bypass << ' ' << hub2 << ' ' << k << '\n';
  for (int i = 0; i < k; ++i) {
    std::cout << hub2 << ' ' << (b_base + i) << " 1\n";
    std::cout << (b_base + i) << ' ' << t << " 1\n";
  }
  return 0;
}
