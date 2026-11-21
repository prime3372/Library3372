// Repository: Library3372
// File: benchmarks/graph/scc_graph/solutions/library3372.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/graph/scc_graph/solutions/library3372.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <iostream>
#include <utility>
#include <vector>

#include "library3372/graph/scc_graph.hpp"
#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n, m;
  std::cin >> n >> m;
  std::vector<std::pair<int, int>> edges(m);
  for (int i = 0; i < m; ++i) std::cin >> edges[i].first >> edges[i].second;

  library3372::internal::Timer timer;
  library3372::SccGraph g(n);
  for (const auto& e : edges) g.add_edge(e.first, e.second);
  double add_sec = timer.elapsed();

  timer.reset();
  g.build_ids();
  double build_sec = timer.elapsed();

  timer.reset();
  g.build();
  double groups_sec = timer.elapsed();

  const auto& ids = g.ids();
  int k = g.count_groups();
  const auto& gs = g.groups();

  for (const auto& e : edges) {
    if (ids[e.first] > ids[e.second]) return 1;
  }

  unsigned long long groups_hash = 0;
  for (const auto& comp : gs) {
    unsigned long long gh = (unsigned long long)comp.size();
    for (int v : comp) {
      gh = library3372::internal::hash_combine(gh, (unsigned long long)v);
    }
    groups_hash += gh;
  }
  unsigned long long ans_hash =
      library3372::internal::hash_combine(0ULL, (unsigned long long)k);
  ans_hash = library3372::internal::hash_combine(ans_hash, groups_hash);

  std::cout << "hash " << ans_hash << "\n";
  std::cout << "time add " << add_sec << "\n";
  std::cout << "time build " << build_sec << "\n";
  std::cout << "time groups " << groups_sec << "\n";
  return 0;
}
