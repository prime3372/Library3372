// Repository: Library3372
// File: benchmarks/graph/scc_graph/generators/path_cycle.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/graph/scc_graph/generators/path_cycle.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

int main(int argc, char** argv) {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n = std::atoi(argv[1]);
  int step = std::atoi(argv[2]);

  std::mt19937_64 rng(0x13198a2e03707344ULL ^ ((unsigned long long)n << 32) ^
                      (unsigned long long)step);
  std::vector<int> perm(n);
  std::iota(perm.begin(), perm.end(), 0);
  std::shuffle(perm.begin(), perm.end(), rng);

  std::vector<std::pair<int, int>> edges;
  edges.reserve(n * 2);
  for (int i = 0; i + 1 < n; ++i) {
    edges.emplace_back(perm[i], perm[i + 1]);
  }
  if (step > 0) {
    for (int i = step; i < n; i += step) {
      edges.emplace_back(perm[i], perm[i - step / 2]);
    }
  }

  std::cout << n << " " << edges.size() << "\n";
  for (const auto& e : edges) {
    std::cout << e.first << " " << e.second << "\n";
  }
  return 0;
}
