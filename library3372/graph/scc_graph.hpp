// Repository: Library3372
// File: library3372/graph/scc_graph.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/graph/scc_graph.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_GRAPH_SCC_GRAPH_HPP_
#define LIBRARY3372_LIBRARY3372_GRAPH_SCC_GRAPH_HPP_

#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

namespace library3372 {

// Tarjan's strongly connected components algorithm
// https://en.wikipedia.org/wiki/Tarjan%27s_strongly_connected_components_algorithm
class SccGraph {
 public:
  SccGraph()
      : n_(0), count_groups_(0), ids_built_(false), groups_built_(false) {}

  explicit SccGraph(int n)
      : n_(n), count_groups_(n), ids_built_(false), groups_built_(false) {
    assert(n >= 0);
  }

  void add_edge(int from, int to) {
    assert(0 <= from && from < n_);
    assert(0 <= to && to < n_);
    edges_.emplace_back(from, to);
    ids_built_ = false;
    groups_built_ = false;
  }

  void build_ids() {
    if (ids_built_) return;
    groups_.clear();
    groups_built_ = false;
    ids_built_ = true;

    if (n_ == 0) {
      ids_.clear();
      count_groups_ = 0;
      return;
    }

    ids_.resize(n_);
    int m = (int)edges_.size();
    std::vector<int> start(n_ + 1, 0);
    for (const auto& e : edges_) ++start[e.first + 1];
    for (int i = 0; i < n_; ++i) start[i + 1] += start[i];

    std::vector<int> elist(m);
    std::vector<int> counter = start;
    for (const auto& e : edges_) elist[counter[e.first]++] = e.second;

    int now_ord = 0;
    int group_num = 0;
    std::vector<int> ord(n_, -1);
    std::vector<int> low(n_);
    std::vector<int> visited;
    visited.reserve(n_);

    auto dfs = [&](auto& self, int v) -> void {
      low[v] = ord[v] = now_ord++;
      visited.push_back(v);
      for (int i = start[v]; i < start[v + 1]; ++i) {
        int to = elist[i];
        if (ord[to] == -1) {
          self(self, to);
          low[v] = std::min(low[v], low[to]);
        } else {
          low[v] = std::min(low[v], ord[to]);
        }
      }
      if (low[v] == ord[v]) {
        while (true) {
          int u = visited.back();
          visited.pop_back();
          ord[u] = n_;
          ids_[u] = group_num;
          if (u == v) break;
        }
        ++group_num;
      }
    };

    for (int i = 0; i < n_; ++i) {
      if (ord[i] == -1) dfs(dfs, i);
    }

    for (int& id : ids_) id = group_num - 1 - id;
    count_groups_ = group_num;
  }

  void build() {
    if (groups_built_) return;
    build_ids();

    std::vector<int> counts(count_groups_, 0);
    for (int id : ids_) ++counts[id];
    groups_.assign(count_groups_, {});
    for (int i = 0; i < count_groups_; ++i) groups_[i].reserve(counts[i]);
    for (int i = 0; i < n_; ++i) groups_[ids_[i]].push_back(i);
    groups_built_ = true;
  }

  int count_groups() const {
    assert(ids_built_);
    return count_groups_;
  }

  const std::vector<int>& ids() const {
    assert(ids_built_);
    return ids_;
  }

  const std::vector<std::vector<int>>& groups() const {
    assert(groups_built_);
    return groups_;
  }

 private:
  int n_;
  std::vector<std::pair<int, int>> edges_;
  std::vector<int> ids_;
  std::vector<std::vector<int>> groups_;
  int count_groups_;
  bool ids_built_;
  bool groups_built_;
};

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_GRAPH_SCC_GRAPH_HPP_
