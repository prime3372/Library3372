// Repository: Library3372
// File: benchmarks/graph/scc_graph/solutions/naive.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/graph/scc_graph/solutions/naive.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <iostream>
#include <queue>
#include <utility>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n, m;
  std::cin >> n >> m;
  std::vector<std::pair<int, int>> edges(m);
  for (int i = 0; i < m; ++i) std::cin >> edges[i].first >> edges[i].second;

  if (n > 3000) {
    std::cout << "skip N > 3000\n";
    return 0;
  }

  library3372::internal::Timer timer;
  std::vector<std::vector<int>> adj(n);
  for (const auto& e : edges) adj[e.first].push_back(e.second);
  double add_sec = timer.elapsed();

  timer.reset();
  std::vector<std::vector<char>> reach(n, std::vector<char>(n, 0));
  std::queue<int> q;
  for (int s = 0; s < n; ++s) {
    reach[s][s] = 1;
    q.push(s);
    while (!q.empty()) {
      int u = q.front();
      q.pop();
      for (int to : adj[u]) {
        if (!reach[s][to]) {
          reach[s][to] = 1;
          q.push(to);
        }
      }
    }
  }

  std::vector<int> ids(n, -1);
  int k = 0;
  for (int i = 0; i < n; ++i) {
    if (ids[i] != -1) continue;
    for (int j = i; j < n; ++j) {
      if (reach[i][j] && reach[j][i]) ids[j] = k;
    }
    ++k;
  }
  double build_sec = timer.elapsed();

  timer.reset();
  std::vector<std::vector<int>> gs(k);
  for (int i = 0; i < n; ++i) gs[ids[i]].push_back(i);
  double groups_sec = timer.elapsed();

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
