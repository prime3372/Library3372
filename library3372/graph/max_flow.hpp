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

// Dinic's algorithm with HLPP fallback
// https://en.wikipedia.org/wiki/Dinic%27s_algorithm
// https://en.wikipedia.org/wiki/Push%E2%80%93relabel_maximum_flow_algorithm
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
    long long nm = (long long)n + (long long)pos_.size();
    long long dfs_calls = 0;
    int phases = 0;
    bool hit_limit = false;

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
      if ((++dfs_calls > 4LL * nm && phases >= 8) || dfs_calls > 16LL * nm) {
        hit_limit = true;
        return 0;
      }
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
        if (d > 0) {
          e.cap = (Cap)(e.cap + d);
          g_[e.to][e.rev].cap = (Cap)(g_[e.to][e.rev].cap - d);
          res = (Cap)(res + d);
          if (res == up) return res;
        }
        if (hit_limit) return res;
      }
      level[v] = n;
      return res;
    };

    Cap flow = 0;
    while (flow < flow_limit) {
      bfs();
      if (level[t] == -1) break;
      ++phases;
      std::fill(iter.begin(), iter.end(), 0);
      Cap f = dfs(dfs, t, (Cap)(flow_limit - flow));
      flow = (Cap)(flow + f);
      if (hit_limit) {
        flow = (Cap)(flow + hlpp_rem(s, t, (Cap)(flow_limit - flow)));
        break;
      }
      if (f == 0) break;
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

  Cap hlpp_rem(int orig_s, int t, Cap rem_limit) {
    if (rem_limit == 0) return 0;
    int orig_n = (int)g_.size();
    int vs = orig_n;
    int n = orig_n + 1;
    g_.emplace_back();
    int orig_s_rev = (int)g_[orig_s].size();
    g_[vs].push_back({orig_s, orig_s_rev, rem_limit});
    g_[orig_s].push_back({vs, 0, 0});

    std::vector<Cap> ex(n, 0);
    std::vector<int> h(n, n);
    std::vector<int> gap(n + 1, 0);
    std::vector<int> arc(n, 0);
    std::vector<int> que(n);
    std::vector<std::vector<int>> buckets(n);
    int max_h = 0;

    auto global_relabel = [&](int target, int forbidden, bool use_gap) {
      std::fill(h.begin(), h.end(), n);
      if (use_gap) std::fill(gap.begin(), gap.end(), 0);
      std::fill(arc.begin(), arc.end(), 0);
      for (int i = 0; i < n; ++i) buckets[i].clear();
      h[target] = 0;
      int qh = 0;
      int qt = 0;
      que[qt++] = target;
      max_h = 0;
      while (qh < qt) {
        int u = que[qh++];
        int nh = h[u] + 1;
        if (nh >= n) continue;
        for (const InternalEdge& e : g_[u]) {
          if (e.to == forbidden) continue;
          if (h[e.to] == n && g_[e.to][e.rev].cap > 0) {
            h[e.to] = nh;
            if (use_gap) ++gap[nh];
            if (e.to != target && ex[e.to] > 0) {
              buckets[nh].push_back(e.to);
              if (nh > max_h) max_h = nh;
            }
            que[qt++] = e.to;
          }
        }
      }
      if (use_gap) gap[0] = 1;
    };

    auto run_phase = [&](int target, int forbidden, bool use_gap) {
      int relabels = 0;
      while (max_h >= 0) {
        while (max_h >= 0 && buckets[max_h].empty()) --max_h;
        if (max_h < 0) break;
        int u = buckets[max_h].back();
        buckets[max_h].pop_back();
        if (ex[u] == 0 || h[u] >= n) continue;

        while (ex[u] > 0) {
          if (arc[u] < (int)g_[u].size()) {
            InternalEdge& e = g_[u][arc[u]];
            if (e.cap > 0 && h[u] == h[e.to] + 1) {
              Cap d = ex[u] < e.cap ? ex[u] : e.cap;
              e.cap = (Cap)(e.cap - d);
              g_[e.to][e.rev].cap = (Cap)(g_[e.to][e.rev].cap + d);
              ex[u] = (Cap)(ex[u] - d);
              if (e.to != target && e.to != forbidden && ex[e.to] == 0 &&
                  h[e.to] < n) {
                buckets[h[e.to]].push_back(e.to);
              }
              ex[e.to] = (Cap)(ex[e.to] + d);
            } else {
              ++arc[u];
            }
          } else {
            if (use_gap) {
              int old_h = h[u];
              --gap[old_h];
              if (gap[old_h] == 0) {
                for (int v = 0; v < orig_n; ++v) {
                  if (old_h < h[v] && h[v] < n) {
                    --gap[h[v]];
                    h[v] = n;
                  }
                }
                h[u] = n;
                break;
              }
            }
            int min_nh = n;
            for (const InternalEdge& e : g_[u]) {
              if (e.to != u && e.cap > 0 && h[e.to] + 1 < min_nh) {
                min_nh = h[e.to] + 1;
              }
            }
            h[u] = min_nh;
            arc[u] = 0;
            ++relabels;
            if (min_nh < n) {
              if (use_gap) ++gap[min_nh];
              if (min_nh > max_h) max_h = min_nh;
            } else {
              break;
            }
            if (relabels >= 4 * n) {
              relabels = 0;
              global_relabel(target, forbidden, use_gap);
              break;
            }
          }
        }
      }
    };

    global_relabel(t, vs, true);
    h[vs] = n;
    if (h[orig_s] < n) {
      InternalEdge& e = g_[vs][0];
      Cap d = e.cap;
      e.cap = 0;
      g_[orig_s][e.rev].cap = (Cap)(g_[orig_s][e.rev].cap + d);
      buckets[h[orig_s]].push_back(orig_s);
      if (h[orig_s] > max_h) max_h = h[orig_s];
      ex[orig_s] = d;
    }
    run_phase(t, vs, true);

    Cap pushed = ex[t];
    global_relabel(vs, t, false);
    run_phase(vs, t, false);

    g_[orig_s].pop_back();
    g_.pop_back();
    return pushed;
  }

  std::vector<std::pair<int, int>> pos_;
  std::vector<std::vector<InternalEdge>> g_;
};

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_GRAPH_MAX_FLOW_HPP_
