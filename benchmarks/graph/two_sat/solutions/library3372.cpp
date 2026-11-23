// Repository: Library3372
// File: benchmarks/graph/two_sat/solutions/library3372.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/graph/two_sat/solutions/library3372.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <iostream>
#include <vector>

#include "library3372/graph/two_sat.hpp"
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

  library3372::internal::Timer timer;
  library3372::TwoSat ts(n);
  for (const auto& c : clauses) ts.add_clause(c.i, c.f, c.j, c.g);
  double add_sec = timer.elapsed();

  timer.reset();
  bool sat = ts.satisfiable();
  double solve_sec = timer.elapsed();

  if (sat) {
    const auto& ans = ts.answer();
    if ((int)ans.size() != n) return 1;
    for (const auto& c : clauses) {
      if (ans[c.i] != c.f && ans[c.j] != c.g) return 1;
    }
  }

  std::cout << "hash " << (sat ? 1ULL : 0ULL) << "\n";
  std::cout << "time add " << add_sec << "\n";
  std::cout << "time solve " << solve_sec << "\n";
  return 0;
}
