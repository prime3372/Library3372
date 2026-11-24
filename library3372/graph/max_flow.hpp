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
#include <vector>

#include "library3372/internal/type_traits/is_integer.hpp"

namespace library3372 {

// Dinic's algorithm and HLPP fallback
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

  explicit MaxFlow(int n) : n_(n) { assert(n >= 0); }

  int add_edge(int from, int to, Cap cap) {
    assert(!flow_called_);
    assert(0 <= from && from < n_);
    assert(0 <= to && to < n_);
    if constexpr (internal::is_signed_integer_v<Cap>) assert(cap >= 0);
    int id = (int)edges_.size();
    edges_.push_back({from, to, cap, 0});
    if (cap > 1) all_unit_ = false;
    return id;
  }

  Edge get_edge(int i) const {
    assert(0 <= i && i < (int)edges_.size());
    Edge e = edges_[i];
    e.flow = edge_flow(i);
    return e;
  }

  std::vector<Edge> edges() const {
    std::vector<Edge> result = edges_;
    if (built_) {
      for (int i = 0; i < (int)result.size(); ++i) {
        result[i].flow = edge_flow(i);
      }
    }
    return result;
  }

  Cap flow(int s, int t) { return flow(s, t, std::numeric_limits<Cap>::max()); }

  Cap flow(int s, int t, Cap flow_limit) {
    assert(0 <= s && s < n_);
    assert(0 <= t && t < n_);
    assert(s != t);
    if constexpr (internal::is_signed_integer_v<Cap>) assert(flow_limit >= 0);
    flow_called_ = true;
    if (flow_limit == 0) return 0;

    if (!built_) build_csr();
    if (csr_is_unit_) return flow_unit(s, t, flow_limit);
    return flow_general(s, t, flow_limit);
  }

  std::vector<bool> min_cut(int s) const {
    assert(0 <= s && s < n_);
    std::vector<int> level(n_, -1);
    std::vector<int> que(n_);
    if (built_) {
      if (csr_is_unit_) {
        dinic_bfs(s, -1, ulist_, level, que,
                  [](const UnitEdge& e) { return (e.rev_cap & 1U) != 0; });
      } else {
        dinic_bfs(s, -1, elist_, level, que,
                  [](const GeneralEdge& e) { return e.cap > 0; });
      }
    } else {
      std::vector<std::vector<int>> adj(n_);
      for (const Edge& e : edges_) {
        if (e.cap > 0) adj[e.from].push_back(e.to);
      }
      int qh = 0;
      int qt = 0;
      level[s] = 0;
      que[qt++] = s;
      while (qh < qt) {
        for (int to : adj[que[qh++]]) {
          if (level[to] < 0) {
            level[to] = 0;
            que[qt++] = to;
          }
        }
      }
    }
    std::vector<bool> visited(n_);
    for (int i = 0; i < n_; ++i) visited[i] = level[i] >= 0;
    return visited;
  }

 private:
  struct GeneralEdge {
    int to;
    int rev;
    Cap cap;
  };

  struct UnitEdge {
    int to;
    unsigned int rev_cap;
  };

  Cap edge_flow(int i) const {
    if (!built_) return 0;
    const Edge& e = edges_[i];
    if (csr_is_unit_) {
      return (Cap)(e.cap - (Cap)(ulist_[edge_pos_[i]].rev_cap & 1U));
    }
    return (Cap)(e.cap - elist_[edge_pos_[i]].cap);
  }

  void build_csr() {
    int m = (int)edges_.size();
    start_.assign(n_ + 1, 0);
    for (const Edge& e : edges_) {
      ++start_[e.from + 1];
      ++start_[e.to + 1];
    }
    for (int i = 0; i < n_; ++i) start_[i + 1] += start_[i];

    std::vector<int> cur = start_;
    edge_pos_.resize(m);
    csr_is_unit_ = all_unit_;
    elist_.resize(all_unit_ ? 0 : 2 * m);
    ulist_.resize(all_unit_ ? 2 * m : 0);
    for (int i = 0; i < m; ++i) {
      const Edge& e = edges_[i];
      int ui = cur[e.from]++;
      int vi = cur[e.to]++;
      edge_pos_[i] = ui;
      if (all_unit_) {
        ulist_[ui] = {e.to, ((unsigned int)vi << 1) | (unsigned int)e.cap};
        ulist_[vi] = {e.from, (unsigned int)ui << 1};
      } else {
        elist_[ui] = {e.to, vi, e.cap};
        elist_[vi] = {e.from, ui, 0};
      }
    }
    built_ = true;
  }

  template <typename EdgeT, typename HasCap>
  void dinic_bfs(int s, int t, const std::vector<EdgeT>& list,
                 std::vector<int>& level, std::vector<int>& que,
                 HasCap has_cap) const {
    std::fill(level.begin(), level.end(), -1);
    level[s] = 0;
    int qh = 0;
    int qt = 0;
    que[qt++] = s;
    while (qh < qt) {
      int v = que[qh++];
      int next_level = level[v] + 1;
      for (int i = start_[v], end = start_[v + 1]; i < end; ++i) {
        const EdgeT& e = list[i];
        if (!has_cap(e) || level[e.to] >= 0) continue;
        level[e.to] = next_level;
        if (e.to == t) return;
        que[qt++] = e.to;
      }
    }
  }

  Cap flow_unit(int s, int t, Cap flow_limit) {
    std::vector<int> level(n_);
    std::vector<int> iter(n_);
    std::vector<int> que(n_);
    auto has_cap = [](const UnitEdge& e) { return (e.rev_cap & 1U) != 0; };

    auto dfs = [&](auto& self, int v) -> bool {
      if (v == s) return true;
      int target_level = level[v] - 1;
      for (int &i = iter[v], end = start_[v + 1]; i < end;) {
        UnitEdge& e = ulist_[i];
        ++i;
        if (level[e.to] == target_level) {
          unsigned int rev = e.rev_cap >> 1;
          if ((ulist_[rev].rev_cap & 1U) && self(self, e.to)) {
            e.rev_cap |= 1U;
            ulist_[rev].rev_cap &= ~1U;
            return true;
          }
        }
      }
      level[v] = n_;
      return false;
    };

    Cap flow = 0;
    while (flow < flow_limit) {
      dinic_bfs(s, t, ulist_, level, que, has_cap);
      if (level[t] == -1) break;
      std::copy(start_.begin(), start_.begin() + n_, iter.begin());
      int target_level = level[t] - 1;
      Cap pushed = 0;
      for (int i = start_[t], end = start_[t + 1];
           i < end && flow + pushed < flow_limit; ++i) {
        UnitEdge& e = ulist_[i];
        if (level[e.to] == target_level) {
          unsigned int rev = e.rev_cap >> 1;
          if ((ulist_[rev].rev_cap & 1U) && dfs(dfs, e.to)) {
            e.rev_cap |= 1U;
            ulist_[rev].rev_cap &= ~1U;
            ++pushed;
          }
        }
      }
      if (pushed == 0) break;
      flow = (Cap)(flow + pushed);
    }
    return flow;
  }

  Cap flow_general(int s, int t, Cap flow_limit) {
    std::vector<int> level(n_);
    std::vector<int> iter(n_);
    std::vector<int> que(n_);
    auto has_cap = [](const GeneralEdge& e) { return e.cap > 0; };
    long long nm = (long long)n_ + (long long)edges_.size();
    long long dfs_calls = 0;
    int phases = 0;
    bool hit_limit = false;

    auto dfs = [&](auto& self, int v, Cap up) -> Cap {
      if (v == s) return up;
      if ((++dfs_calls > 4LL * nm && phases >= 8) || dfs_calls > 16LL * nm) {
        hit_limit = true;
        return 0;
      }
      Cap res = 0;
      int target_level = level[v] - 1;
      for (int &i = iter[v], end = start_[v + 1]; i < end; ++i) {
        GeneralEdge& e = elist_[i];
        if (level[e.to] != target_level) continue;
        Cap rev_cap = elist_[e.rev].cap;
        if (rev_cap == 0) continue;
        Cap rem = (Cap)(up - res);
        Cap d = self(self, e.to, rem < rev_cap ? rem : rev_cap);
        if (d > 0) {
          e.cap = (Cap)(e.cap + d);
          elist_[e.rev].cap = (Cap)(elist_[e.rev].cap - d);
          res = (Cap)(res + d);
          if (res == up) {
            if (d == rev_cap) ++i;
            return res;
          }
        }
        if (hit_limit) return res;
      }
      level[v] = n_;
      return res;
    };

    Cap flow = 0;
    while (flow < flow_limit) {
      dinic_bfs(s, t, elist_, level, que, has_cap);
      if (level[t] == -1) break;
      ++phases;
      std::copy(start_.begin(), start_.begin() + n_, iter.begin());
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

  Cap hlpp_rem(int orig_s, int t, Cap rem_limit) {
    if (rem_limit == 0) return 0;
    int n = n_;
    std::vector<Cap> ex(n, 0);
    std::vector<int> h(n, n);
    std::vector<int> gap(n + 1, 0);
    std::vector<int> arc(start_.begin(), start_.begin() + n);
    std::vector<int> que(n);
    std::vector<std::vector<int>> buckets(n);
    int max_h = 0;
    Cap vs_pushed = 0;

    auto global_relabel = [&](bool phase2) {
      std::fill(h.begin(), h.end(), n);
      if (!phase2) std::fill(gap.begin(), gap.end(), 0);
      std::copy(start_.begin(), start_.begin() + n, arc.begin());
      for (int i = 0; i < n; ++i) buckets[i].clear();
      max_h = 0;
      if (phase2 && vs_pushed == 0) return;

      int root = phase2 ? orig_s : t;
      int root_h = phase2 ? 1 : 0;
      h[root] = root_h;
      if (!phase2) gap[0] = 1;
      if (phase2 && ex[root] > 0) {
        buckets[root_h].push_back(root);
        max_h = root_h;
      }
      int qh = 0;
      int qt = 0;
      que[qt++] = root;
      while (qh < qt) {
        int u = que[qh++];
        int nh = h[u] + 1;
        if (nh >= n) continue;
        for (int i = start_[u], end = start_[u + 1]; i < end; ++i) {
          const GeneralEdge& e = elist_[i];
          if (e.to == t || h[e.to] != n || elist_[e.rev].cap == 0) continue;
          h[e.to] = nh;
          if (!phase2) ++gap[nh];
          if (ex[e.to] > 0) {
            buckets[nh].push_back(e.to);
            if (nh > max_h) max_h = nh;
          }
          que[qt++] = e.to;
        }
      }
    };

    auto run_phase = [&](bool phase2) {
      int relabels = 0;
      while (max_h >= 0) {
        while (max_h >= 0 && buckets[max_h].empty()) --max_h;
        if (max_h < 0) break;
        int u = buckets[max_h].back();
        buckets[max_h].pop_back();
        if (ex[u] == 0 || h[u] >= n) continue;

        while (ex[u] > 0) {
          if (phase2 && u == orig_s && h[orig_s] == 1 && vs_pushed > 0) {
            Cap d = ex[u] < vs_pushed ? ex[u] : vs_pushed;
            vs_pushed = (Cap)(vs_pushed - d);
            ex[u] = (Cap)(ex[u] - d);
            if (ex[u] == 0) break;
          }
          if (arc[u] < start_[u + 1]) {
            GeneralEdge& e = elist_[arc[u]];
            if (e.cap > 0 && h[u] == h[e.to] + 1) {
              Cap d = ex[u] < e.cap ? ex[u] : e.cap;
              e.cap = (Cap)(e.cap - d);
              elist_[e.rev].cap = (Cap)(elist_[e.rev].cap + d);
              ex[u] = (Cap)(ex[u] - d);
              if (e.to != t && ex[e.to] == 0 && h[e.to] < n) {
                buckets[h[e.to]].push_back(e.to);
              }
              ex[e.to] = (Cap)(ex[e.to] + d);
            } else {
              ++arc[u];
            }
          } else {
            if (!phase2) {
              int old_h = h[u];
              --gap[old_h];
              if (gap[old_h] == 0) {
                for (int v = 0; v < n; ++v) {
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
            for (int i = start_[u], end = start_[u + 1]; i < end; ++i) {
              const GeneralEdge& e = elist_[i];
              if (e.to != u && e.cap > 0 && h[e.to] + 1 < min_nh) {
                min_nh = h[e.to] + 1;
              }
            }
            if (phase2 && u == orig_s && vs_pushed > 0 && 1 < min_nh) {
              min_nh = 1;
            }
            h[u] = min_nh;
            arc[u] = start_[u];
            ++relabels;
            if (min_nh < n) {
              if (!phase2) ++gap[min_nh];
              if (min_nh > max_h) max_h = min_nh;
            } else if (phase2) {
              relabels = 0;
              global_relabel(true);
              break;
            } else {
              break;
            }
            if (relabels >= 4 * n) {
              relabels = 0;
              global_relabel(phase2);
              break;
            }
          }
        }
      }
    };

    global_relabel(false);
    if (h[orig_s] < n) {
      vs_pushed = rem_limit;
      ex[orig_s] = rem_limit;
      buckets[h[orig_s]].push_back(orig_s);
      if (h[orig_s] > max_h) max_h = h[orig_s];
      run_phase(false);
    }

    Cap pushed = ex[t];
    global_relabel(true);
    run_phase(true);
    return pushed;
  }

  int n_ = 0;
  bool all_unit_ = true;
  bool flow_called_ = false;
  bool built_ = false;
  bool csr_is_unit_ = false;
  std::vector<Edge> edges_;
  std::vector<int> start_;
  std::vector<int> edge_pos_;
  std::vector<GeneralEdge> elist_;
  std::vector<UnitEdge> ulist_;
};

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_GRAPH_MAX_FLOW_HPP_
