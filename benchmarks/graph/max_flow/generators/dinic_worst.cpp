// Repository: Library3372
// File: benchmarks/graph/max_flow/generators/dinic_worst.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/graph/max_flow/generators/dinic_worst.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cassert>
#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[]) {
  assert(argc >= 4);
  int k = std::atoi(argv[1]);
  int chain_len = std::atoi(argv[2]);
  int p = std::atoi(argv[3]);
  assert(k >= 2 && chain_len >= 2 && p >= 1);

  constexpr long long kInf = 1000000000LL;
  int n = 2 * k + chain_len + 2;
  int s = 0;
  int t = n - 1;
  int x_base = 1;
  int c_base = 1 + k;
  int y_base = 1 + k + chain_len;

  int m = k + (k - 1) + 1 + (chain_len - 1) + 1 + (k - 1) + k * p;
  std::cout << n << ' ' << m << ' ' << s << ' ' << t << '\n';

  // Left ladder forcing O(k) BFS phases.
  for (int r = 0; r < k; ++r) {
    std::cout << s << ' ' << (x_base + r) << ' ' << p << '\n';
  }
  for (int r = 1; r < k; ++r) {
    std::cout << (x_base + r) << ' ' << (x_base + r - 1) << ' ' << kInf << '\n';
  }

  // Long shared chain walked by O(p) unit augmentations per phase.
  std::cout << x_base << ' ' << c_base << ' ' << kInf << '\n';
  for (int i = 0; i + 1 < chain_len; ++i) {
    std::cout << (c_base + i) << ' ' << (c_base + i + 1) << ' ' << kInf << '\n';
  }
  std::cout << (c_base + chain_len - 1) << ' ' << y_base << ' ' << kInf << '\n';

  // Right ladder with p unit-capacity parallel edges per phase.
  for (int r = 0; r + 1 < k; ++r) {
    std::cout << (y_base + r) << ' ' << (y_base + r + 1) << ' ' << kInf << '\n';
  }
  for (int r = 0; r < k; ++r) {
    for (int j = 0; j < p; ++j) {
      std::cout << (y_base + r) << ' ' << t << " 1\n";
    }
  }
  return 0;
}
