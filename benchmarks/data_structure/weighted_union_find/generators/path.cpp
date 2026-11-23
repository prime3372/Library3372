// Repository: Library3372
// File: benchmarks/data_structure/weighted_union_find/generators/path.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/data_structure/weighted_union_find/generators/path.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <numeric>
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

  std::mt19937_64 rng(0x3c6ef372fe94f82bULL);

  std::vector<int> perm(n);
  std::iota(perm.begin(), perm.end(), 0);
  std::shuffle(perm.begin(), perm.end(), rng);

  std::vector<std::pair<int, int>> edges;
  edges.reserve(n - 1);
  for (int i = 0; i + 1 < n; ++i) {
    edges.emplace_back(perm[i], perm[i + 1]);
  }
  std::shuffle(edges.begin(), edges.end(), rng);

  std::uniform_int_distribution<int> vdist(0, n - 1);
  std::uniform_int_distribution<long long> wdist(-1000000000LL, 1000000000LL);
  std::uniform_int_distribution<int> opdist(0, 99);

  std::cout << n << ' ' << q << '\n';
  int emitted = 0;
  for (const auto& [u, v] : edges) {
    if (emitted >= q) break;
    std::cout << "0 " << u << ' ' << v << ' ' << wdist(rng) << '\n';
    ++emitted;
  }

  while (emitted < q) {
    int p = opdist(rng);
    if (p < 25) {
      std::cout << "1 " << vdist(rng) << ' ' << vdist(rng) << '\n';
    } else if (p < 70) {
      std::cout << "2 " << vdist(rng) << ' ' << vdist(rng) << '\n';
    } else if (p < 82) {
      std::cout << "3 " << vdist(rng) << '\n';
    } else if (p < 95) {
      std::cout << "4 " << vdist(rng) << '\n';
    } else {
      std::cout << "5\n";
    }
    ++emitted;
  }
  return 0;
}
