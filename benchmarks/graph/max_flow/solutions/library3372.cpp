// Repository: Library3372
// File: benchmarks/graph/max_flow/solutions/library3372.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/graph/max_flow/solutions/library3372.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <iostream>
#include <vector>

#include "library3372/graph/max_flow.hpp"
#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"

namespace {

struct RawEdge {
  int u;
  int v;
  long long c;
};

}  // namespace

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n, m, s, t;
  std::cin >> n >> m >> s >> t;
  std::vector<RawEdge> raw_edges(m);
  for (int i = 0; i < m; ++i) {
    std::cin >> raw_edges[i].u >> raw_edges[i].v >> raw_edges[i].c;
  }

  library3372::internal::Timer timer;
  library3372::MaxFlow<long long> mf(n);
  for (const auto& e : raw_edges) mf.add_edge(e.u, e.v, e.c);
  double build_sec = timer.elapsed();

  timer.reset();
  long long flow_val = mf.flow(s, t);
  double flow_sec = timer.elapsed();

  timer.reset();
  auto es = mf.edges();
  auto cut = mf.min_cut(s);
  double post_sec = timer.elapsed();

  long long out_s = 0;
  for (const auto& e : es) {
    if (e.from == e.to) continue;
    if (e.from == s) out_s += e.flow;
    if (e.to == s) out_s -= e.flow;
  }

  unsigned long long ans_hash = (unsigned long long)flow_val;
  ans_hash =
      library3372::internal::hash_combine(ans_hash, (unsigned long long)out_s);
  for (bool in_s : cut) {
    ans_hash =
        library3372::internal::hash_combine(ans_hash, in_s ? 1ULL : 0ULL);
  }

  std::cout << "hash " << ans_hash << '\n';
  std::cout << "time build " << build_sec << '\n';
  std::cout << "time flow " << flow_sec << '\n';
  std::cout << "time post " << post_sec << '\n';
  return 0;
}
