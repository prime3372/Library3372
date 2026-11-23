// Repository: Library3372
// File: benchmarks/graph/two_sat/solutions/naive.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/graph/two_sat/solutions/naive.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <iostream>
#include <queue>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"

namespace {

struct Clause {
  int i;
  bool f;
  int j;
  bool g;
};

}  // namespace

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n, m;
  std::cin >> n >> m;
  std::vector<Clause> clauses(m);
  for (int k = 0; k < m; ++k) {
    int f, g;
    std::cin >> clauses[k].i >> f >> clauses[k].j >> g;
    clauses[k].f = f != 0;
    clauses[k].g = g != 0;
  }

  if (n > 3000) {
    std::cout << "skip N > 3000\n";
    return 0;
  }

  library3372::internal::Timer timer;
  std::vector<std::vector<int>> adj(2 * n);
  for (const auto& c : clauses) {
    adj[2 * c.i + (c.f ? 0 : 1)].push_back(2 * c.j + (c.g ? 1 : 0));
    adj[2 * c.j + (c.g ? 0 : 1)].push_back(2 * c.i + (c.f ? 1 : 0));
  }
  double add_sec = timer.elapsed();

  timer.reset();
  std::vector<std::vector<char>> reach(2 * n, std::vector<char>(2 * n, 0));
  std::queue<int> q;
  for (int s = 0; s < 2 * n; ++s) {
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

  bool sat = true;
  for (int i = 0; i < n; ++i) {
    if (reach[2 * i][2 * i + 1] && reach[2 * i + 1][2 * i]) {
      sat = false;
      break;
    }
  }
  double solve_sec = timer.elapsed();

  std::cout << "hash " << (sat ? 1ULL : 0ULL) << "\n";
  std::cout << "time add " << add_sec << "\n";
  std::cout << "time solve " << solve_sec << "\n";
  return 0;
}
