// Repository: Library3372
// File: library3372/graph/max_flow.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/graph/max_flow.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_GRAPH_MAX_FLOW_HPP_
#define LIBRARY3372_LIBRARY3372_GRAPH_MAX_FLOW_HPP_

#include <algorithm>
#include <cassert>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>

#include "library3372/internal/type_traits/is_integer.hpp"

namespace library3372 {

// Dinic's algorithm for maximum flow
// https://en.wikipedia.org/wiki/Dinic%27s_algorithm
template <typename Cap>
  requires(internal::is_integer_v<Cap> && !std::is_same_v<Cap, bool> &&
           !std::is_const_v<Cap> && !std::is_volatile_v<Cap>)
class MaxFlow {
 public:
  struct Edge {
    int from;
    int to;
    Cap cap;
    Cap flow;
  };

  MaxFlow() = default;

  explicit MaxFlow(int n) {
    assert(n >= 0);
    g_.resize(n);
  }

  int add_edge(int from, int to, Cap cap) {
    int n = (int)g_.size();
    assert(0 <= from && from < n);
    assert(0 <= to && to < n);
    if constexpr (internal::is_signed_integer_v<Cap>) assert(cap >= 0);
    int m = (int)pos_.size();
    int from_id = (int)g_[from].size();
    int to_id = (int)g_[to].size();
    if (from == to) ++to_id;
    pos_.emplace_back(from, from_id);
    g_[from].push_back({to, to_id, cap});
    g_[to].push_back({from, from_id, 0});
    return m;
  }

  Edge get_edge(int i) const {
    assert(0 <= i && i < (int)pos_.size());
    const InternalEdge& e = g_[pos_[i].first][pos_[i].second];
    const InternalEdge& re = g_[e.to][e.rev];
    return Edge{pos_[i].first, e.to, (Cap)(e.cap + re.cap), re.cap};
  }

  std::vector<Edge> edges() const {
    int m = (int)pos_.size();
    std::vector<Edge> result;
    result.reserve(m);
    for (int i = 0; i < m; ++i) result.push_back(get_edge(i));
    return result;
  }

  Cap flow(int s, int t) { return flow(s, t, std::numeric_limits<Cap>::max()); }

  Cap flow(int s, int t, Cap flow_limit) {
    int n = (int)g_.size();
    assert(0 <= s && s < n);
    assert(0 <= t && t < n);
    assert(s != t);
    if constexpr (internal::is_signed_integer_v<Cap>) assert(flow_limit >= 0);

    std::vector<int> level(n);
    std::vector<int> iter(n);
    std::vector<int> que;

    auto bfs = [&]() {
      std::fill(level.begin(), level.end(), -1);
      level[s] = 0;
      que.clear();
      que.push_back(s);
      for (int head = 0; head < (int)que.size(); ++head) {
        int v = que[head];
        int next_level = level[v] + 1;
        for (const InternalEdge& e : g_[v]) {
          if (e.cap == 0 || level[e.to] >= 0) continue;
          level[e.to] = next_level;
          if (e.to == t) return;
          que.push_back(e.to);
        }
      }
    };

    auto dfs = [&](auto& self, int v, Cap up) -> Cap {
      if (v == s) return up;
      Cap res = 0;
      int target_level = level[v] - 1;
      int deg = (int)g_[v].size();
      for (int& i = iter[v]; i < deg; ++i) {
        InternalEdge& e = g_[v][i];
        if (level[e.to] != target_level) continue;
        Cap rev_cap = g_[e.to][e.rev].cap;
        if (rev_cap == 0) continue;
        Cap rem = (Cap)(up - res);
        Cap d = self(self, e.to, rem < rev_cap ? rem : rev_cap);
        if (d <= 0) continue;
        e.cap = (Cap)(e.cap + d);
        g_[e.to][e.rev].cap = (Cap)(g_[e.to][e.rev].cap - d);
        res = (Cap)(res + d);
        if (res == up) return res;
      }
      level[v] = n;
      return res;
    };

    Cap flow = 0;
    while (flow < flow_limit) {
      bfs();
      if (level[t] == -1) break;
      std::fill(iter.begin(), iter.end(), 0);
      Cap f = dfs(dfs, t, (Cap)(flow_limit - flow));
      if (f == 0) break;
      flow = (Cap)(flow + f);
    }
    return flow;
  }

  std::vector<bool> min_cut(int s) const {
    int n = (int)g_.size();
    assert(0 <= s && s < n);
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
    Cap cap;
  };

  std::vector<std::pair<int, int>> pos_;
  std::vector<std::vector<InternalEdge>> g_;
};

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_GRAPH_MAX_FLOW_HPP_
