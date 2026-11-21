// Repository: Library3372
// File: library3372/data_structure/union_find.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/data_structure/union_find.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_UNION_FIND_HPP_
#define LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_UNION_FIND_HPP_

#include <cassert>
#include <optional>
#include <utility>
#include <vector>

namespace library3372 {

// Disjoint-set data structure with path compression and union by size
// https://en.wikipedia.org/wiki/Disjoint-set_data_structure
class UnionFind {
 public:
  UnionFind() : count_groups_(0) {}

  explicit UnionFind(int n) : count_groups_(n) {
    assert(n >= 0);
    parent_or_size_.assign(n, -1);
  }

  int find(int x) {
    assert(0 <= x && x < (int)parent_or_size_.size());
    int r = x;
    while (parent_or_size_[r] >= 0) r = parent_or_size_[r];
    while (x != r) {
      int next = parent_or_size_[x];
      parent_or_size_[x] = r;
      x = next;
    }
    return r;
  }

  std::optional<std::pair<int, int>> unite(int x, int y) {
    assert(0 <= x && x < (int)parent_or_size_.size());
    assert(0 <= y && y < (int)parent_or_size_.size());
    x = find(x);
    y = find(y);
    if (x == y) return std::nullopt;
    if (parent_or_size_[x] > parent_or_size_[y]) std::swap(x, y);
    parent_or_size_[x] += parent_or_size_[y];
    parent_or_size_[y] = x;
    --count_groups_;
    return std::pair<int, int>{x, y};
  }

  bool same(int x, int y) {
    assert(0 <= x && x < (int)parent_or_size_.size());
    assert(0 <= y && y < (int)parent_or_size_.size());
    return find(x) == find(y);
  }

  int size(int x) {
    assert(0 <= x && x < (int)parent_or_size_.size());
    return -parent_or_size_[find(x)];
  }

  int count_groups() const { return count_groups_; }

  std::vector<std::vector<int>> groups() {
    int n = (int)parent_or_size_.size();
    std::vector<int> group_id(n, -1);
    std::vector<std::vector<int>> result(count_groups_);
    int next_id = 0;
    for (int i = 0; i < n; ++i) {
      int r = find(i);
      if (group_id[r] == -1) {
        group_id[r] = next_id++;
        result[group_id[r]].reserve(-parent_or_size_[r]);
      }
      result[group_id[r]].push_back(i);
    }
    return result;
  }

 private:
  std::vector<int> parent_or_size_;
  int count_groups_;
};

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_UNION_FIND_HPP_
