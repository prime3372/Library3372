// Repository: Library3372
// File: benchmarks/graph/max_flow/generators/hlpp_worst.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/graph/max_flow/generators/hlpp_worst.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <vector>

namespace {

struct RawEdge {
  int u;
  int v;
  long long c;
};

}  // namespace

int main(int argc, char* argv[]) {
  assert(argc >= 6);
  int b = std::atoi(argv[1]);
  int l = std::atoi(argv[2]);
  int k1 = std::atoi(argv[3]);
  int chain1 = std::atoi(argv[4]);
  int p1 = std::atoi(argv[5]);
  assert(b >= 2 && l >= 1 && k1 >= 2 && chain1 >= 2 && p1 >= 1);

  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  constexpr long long inf = 1000000000LL;
  int l0 = 2 * k1 + chain1 + 20;
  int x_base = 1;
  int c1_base = x_base + k1;
  int y_base = c1_base + chain1;
  int b_base = y_base + k1;
  int u_base = b_base + l0 + l;
  int t = u_base + b;
  int n = t + 1;

  std::vector<RawEdge> edges;

  // Dinic worst-case front-end to trigger HLPP fallback before reaching the
  // dense core.
  for (int r = 0; r < k1; ++r) edges.push_back({0, x_base + r, p1});
  for (int r = 1; r < k1; ++r) {
    edges.push_back({x_base + r, x_base + r - 1, inf});
  }
  edges.push_back({x_base, c1_base, inf});
  for (int i = 0; i + 1 < chain1; ++i) {
    edges.push_back({c1_base + i, c1_base + i + 1, inf});
  }
  edges.push_back({c1_base + chain1 - 1, y_base, inf});
  for (int r = 0; r + 1 < k1; ++r) {
    edges.push_back({y_base + r, y_base + r + 1, inf});
  }
  for (int r = 0; r < k1; ++r) {
    for (int j = 0; j < p1; ++j) edges.push_back({y_base + r, t, 1});
  }

  // Backbone keeping all heights 1..(l0 + l) non-empty to prevent gap pruning.
  edges.push_back({b_base, t, 2LL * l});
  for (int i = 1; i < l0 + l; ++i) {
    edges.push_back({b_base + i, b_base + i - 1, 2LL * l});
  }

  // Dense core forced to relabel step-by-step up the backbone.
  for (int i = 0; i < b; ++i) {
    edges.push_back({0, u_base + i, inf});
    for (int j = 0; j < b; ++j) {
      if (i != j) edges.push_back({u_base + i, u_base + j, inf});
    }
  }
  for (int k = 0; k < l; ++k) {
    edges.push_back({u_base, b_base + (l0 - 1) + k, 1});
  }

  std::cout << n << ' ' << edges.size() << " 0 " << t << '\n';
  for (const auto& e : edges) {
    std::cout << e.u << ' ' << e.v << ' ' << e.c << '\n';
  }
  return 0;
}
