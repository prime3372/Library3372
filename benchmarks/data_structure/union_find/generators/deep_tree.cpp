// Repository: Library3372
// File: benchmarks/data_structure/union_find/generators/deep_tree.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/data_structure/union_find/generators/deep_tree.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <random>
#include <utility>
#include <vector>

int main(int argc, char* argv[]) {
  assert(argc >= 3);
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n = std::atoi(argv[1]);
  int q = std::atoi(argv[2]);
  assert(n >= 1 && q >= 1);

  std::vector<std::pair<int, int>> build_edges;
  build_edges.reserve(n);
  // Build power-of-two binomial-like trees without touching deep leaves.
  for (int step = 1; step < n; step *= 2) {
    for (int i = 0; i + step < n; i += 2 * step) {
      build_edges.emplace_back(i, i + step);
    }
  }

  std::mt19937_64 rng(0x13198a2e03707344ULL);
  std::uniform_int_distribution<int> vdist(0, n - 1);
  std::uniform_int_distribution<int> opdist(0, 99);

  std::cout << n << ' ' << q << '\n';
  int emitted = 0;
  for (const auto& [u, v] : build_edges) {
    if (emitted >= q) break;
    std::cout << "0 " << u << ' ' << v << '\n';
    ++emitted;
  }

  // First pass queries all vertices in reverse order so deep leaves are hit
  // before compression.
  for (int i = n - 1; i >= 0 && emitted < q; --i) {
    std::cout << "3 " << i << '\n';
    ++emitted;
  }

  while (emitted < q) {
    int p = opdist(rng);
    if (p < 40) {
      std::cout << "1 " << vdist(rng) << ' ' << vdist(rng) << '\n';
    } else if (p < 70) {
      std::cout << "2 " << vdist(rng) << '\n';
    } else if (p < 95) {
      std::cout << "3 " << vdist(rng) << '\n';
    } else {
      std::cout << "4\n";
    }
    ++emitted;
  }
  return 0;
}
