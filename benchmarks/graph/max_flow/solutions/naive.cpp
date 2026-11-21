// Repository: Library3372
// File: benchmarks/graph/max_flow/solutions/naive.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/graph/max_flow/solutions/naive.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <iostream>
#include <limits>
#include <utility>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"

namespace {

struct RawEdge {
  int u;
  int v;
  long long c;
};

class EdmondsKarp {
 public:
  struct Edge {
    int from;
    int to;
    long long cap;
    long long flow;
  };

  explicit EdmondsKarp(int n) : g_(n) {}

  void add_edge(int from, int to, long long cap) {
    int from_id = (int)g_[from].size();
    int to_id = (int)g_[to].size();
    if (from == to) ++to_id;
    pos_.emplace_back(from, from_id);
    g_[from].push_back({to, to_id, cap});
    g_[to].push_back({from, from_id, 0});
  }

  std::vector<Edge> edges() const {
    int m = (int)pos_.size();
    std::vector<Edge> result;
    result.reserve(m);
    for (int i = 0; i < m; ++i) {
      const InternalEdge& e = g_[pos_[i].first][pos_[i].second];
      const InternalEdge& re = g_[e.to][e.rev];
      result.push_back({pos_[i].first, e.to, e.cap + re.cap, re.cap});
    }
    return result;
  }

  long long flow(int s, int t) {
    int n = (int)g_.size();
    long long total = 0;
    std::vector<int> pv(n);
    std::vector<int> pe(n);
    std::vector<int> que;

    while (true) {
      std::fill(pv.begin(), pv.end(), -1);
      pv[s] = s;
      que.clear();
      que.push_back(s);
      for (int head = 0; head < (int)que.size() && pv[t] == -1; ++head) {
        int v = que[head];
        for (int i = 0; i < (int)g_[v].size(); ++i) {
          const InternalEdge& e = g_[v][i];
          if (e.cap > 0 && pv[e.to] == -1) {
            pv[e.to] = v;
            pe[e.to] = i;
            que.push_back(e.to);
            if (e.to == t) break;
          }
        }
      }
      if (pv[t] == -1) break;

      long long add = std::numeric_limits<long long>::max();
      for (int v = t; v != s; v = pv[v]) {
        long long c = g_[pv[v]][pe[v]].cap;
        if (c < add) add = c;
      }
      for (int v = t; v != s; v = pv[v]) {
        InternalEdge& e = g_[pv[v]][pe[v]];
        e.cap -= add;
        g_[v][e.rev].cap += add;
      }
      total += add;
    }
    return total;
  }

  std::vector<bool> min_cut(int s) const {
    int n = (int)g_.size();
    std::vector<bool> visited(n, false);
    std::vector<int> que;
    visited[s] = true;
    que.push_back(s);
    for (int head = 0; head < (int)que.size(); ++head) {
      int v = que[head];
      for (const InternalEdge& e : g_[v]) {
        if (e.cap > 0 && !visited[e.to]) {
          visited[e.to] = true;
          que.push_back(e.to);
        }
      }
    }
    return visited;
  }

 private:
  struct InternalEdge {
    int to;
    int rev;
    long long cap;
  };

  std::vector<std::pair<int, int>> pos_;
  std::vector<std::vector<InternalEdge>> g_;
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

  if (n > 3000 || m > 10000) {
    std::cout << "skip N > 3000 or M > 10000\n";
    return 0;
  }

  library3372::internal::Timer timer;
  EdmondsKarp ek(n);
  for (const auto& e : raw_edges) ek.add_edge(e.u, e.v, e.c);
  double build_sec = timer.elapsed();

  timer.reset();
  long long flow_val = ek.flow(s, t);
  double flow_sec = timer.elapsed();

  timer.reset();
  auto es = ek.edges();
  auto cut = ek.min_cut(s);
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
