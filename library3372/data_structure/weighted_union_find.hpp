// Repository: Library3372
// File: library3372/data_structure/weighted_union_find.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/data_structure/weighted_union_find.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_WEIGHTED_UNION_FIND_HPP_
#define LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_WEIGHTED_UNION_FIND_HPP_

#include <cassert>
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>

#include "library3372/internal/type_traits/is_integer.hpp"
#include "library3372/internal/type_traits/to_unsigned.hpp"

namespace library3372 {

// Weighted disjoint-set data structure with path compression and union by size
// https://en.wikipedia.org/wiki/Disjoint-set_data_structure
template <typename T>
  requires(std::is_object_v<T> && !std::is_const_v<T> &&
           !std::is_volatile_v<T> && !std::is_same_v<T, bool>)
class WeightedUnionFind {
 public:
  struct UniteResult {
    bool consistent;
    std::optional<std::pair<int, int>> merged;
  };

  WeightedUnionFind() : count_groups_(0) {}

  explicit WeightedUnionFind(int n) : count_groups_(n >= 0 ? n : 0) {
    assert(n >= 0);
    parent_or_size_.assign(n, -1);
    diff_weight_.assign(n, Storage{});
  }

  int find(int x) {
    assert(0 <= x && x < (int)parent_or_size_.size());
    return find_internal(x);
  }

  UniteResult unite(int x, int y, const T& w) {
    assert(0 <= x && x < (int)parent_or_size_.size());
    assert(0 <= y && y < (int)parent_or_size_.size());
    int rx = find_internal(x);
    int ry = find_internal(y);
    if (rx == ry) {
      bool ok = (sub_val(diff_weight_[y], diff_weight_[x]) == (Storage)w);
      return {ok, std::nullopt};
    }
    Storage d = add_val((Storage)w, diff_weight_[x]);
    sub_assign(d, diff_weight_[y]);
    if (parent_or_size_[rx] > parent_or_size_[ry]) {
      std::swap(rx, ry);
      d = sub_val(Storage{}, d);
    }
    parent_or_size_[rx] += parent_or_size_[ry];
    parent_or_size_[ry] = rx;
    diff_weight_[ry] = std::move(d);
    --count_groups_;
    return {true, std::pair<int, int>{rx, ry}};
  }

  bool same(int x, int y) {
    assert(0 <= x && x < (int)parent_or_size_.size());
    assert(0 <= y && y < (int)parent_or_size_.size());
    return find_internal(x) == find_internal(y);
  }

  T diff(int x, int y) {
    assert(0 <= x && x < (int)parent_or_size_.size());
    assert(0 <= y && y < (int)parent_or_size_.size());
    int rx = find_internal(x);
    int ry = find_internal(y);
    assert(rx == ry);
    return (T)sub_val(diff_weight_[y], diff_weight_[x]);
  }

  int size(int x) {
    assert(0 <= x && x < (int)parent_or_size_.size());
    return -parent_or_size_[find_internal(x)];
  }

  int count_groups() const { return count_groups_; }

  std::vector<std::vector<int>> groups() {
    int n = (int)parent_or_size_.size();
    std::vector<int> group_id(n, -1);
    std::vector<std::vector<int>> result(count_groups_);
    int next_id = 0;
    for (int i = 0; i < n; ++i) {
      int r = find_internal(i);
      int id = group_id[r];
      if (id == -1) {
        id = group_id[r] = next_id++;
        result[id].reserve(-parent_or_size_[r]);
      }
      result[id].push_back(i);
    }
    return result;
  }

 private:
  using Storage = internal::to_unsigned_t<T>;

  static void add_assign(Storage& a, const Storage& b) {
    if constexpr (sizeof(Storage) < sizeof(int) &&
                  internal::is_integer_v<Storage>) {
      a = (Storage)(a + b);
    } else {
      a += b;
    }
  }

  static void sub_assign(Storage& a, const Storage& b) {
    if constexpr (sizeof(Storage) < sizeof(int) &&
                  internal::is_integer_v<Storage>) {
      a = (Storage)(a - b);
    } else {
      a -= b;
    }
  }

  static Storage add_val(Storage a, const Storage& b) {
    add_assign(a, b);
    return a;
  }

  static Storage sub_val(Storage a, const Storage& b) {
    sub_assign(a, b);
    return a;
  }

  int find_internal(int x) {
    int p = parent_or_size_[x];
    if (p < 0) return x;
    int r = parent_or_size_[p];
    if (r < 0) return p;
    Storage sum = add_val(diff_weight_[x], diff_weight_[p]);
    while (parent_or_size_[r] >= 0) {
      add_assign(sum, diff_weight_[r]);
      r = parent_or_size_[r];
    }
    int curr = x;
    while (parent_or_size_[curr] != r) {
      int next = parent_or_size_[curr];
      Storage w = std::move(diff_weight_[curr]);
      parent_or_size_[curr] = r;
      diff_weight_[curr] = sum;
      sub_assign(sum, w);
      curr = next;
    }
    return r;
  }

  std::vector<int> parent_or_size_;
  std::vector<Storage> diff_weight_;
  int count_groups_;
};

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_WEIGHTED_UNION_FIND_HPP_
