// Repository: Library3372
// File: library3372/data_structure/dual_fenwick_tree.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/data_structure/dual_fenwick_tree.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_DUAL_FENWICK_TREE_HPP_
#define LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_DUAL_FENWICK_TREE_HPP_

#include <cassert>
#include <utility>
#include <vector>

#include "library3372/internal/algebra/abelian_group.hpp"

namespace library3372 {

// Dual Fenwick tree (difference array on BIT) for range-add and point-get queries
// https://en.wikipedia.org/wiki/Fenwick_tree
template <typename T>
  requires internal::AbelianGroup<T>
class DualFenwickTree {
 public:
  DualFenwickTree() = default;

  explicit DualFenwickTree(int n) : n_(n >= 0 ? n : 0) {
    assert(n >= 0);
    if (n_ > 0) data_.assign(n_ + 1, Storage{});
  }

  explicit DualFenwickTree(const std::vector<T>& a) : n_((int)a.size()) {
    if (n_ == 0) return;
    data_.resize(n_ + 1);
    Storage prev{};
    for (int i = 0; i < n_; ++i) {
      Storage cur = (Storage)a[i];
      data_[i + 1] = Adapter::sub_val(cur, prev);
      prev = std::move(cur);
    }
    for (int i = 1; i <= n_; ++i) {
      int j = i + (i & -i);
      if (j <= n_) Adapter::add_assign(data_[j], data_[i]);
    }
  }

  void add(int p, const T& x) {
    assert(0 <= p && p < n_);
    add(p, p + 1, x);
  }

  void add(int l, int r, const T& x) {
    assert(0 <= l && l <= r && r <= n_);
    if (l == r) return;
    Storage sx = (Storage)x;
    int i = l + 1;
    if (r == n_) {
      for (; i <= n_; i += i & -i) Adapter::add_assign(data_[i], sx);
      return;
    }
    Storage nx = Adapter::neg_val(sx);
    int j = r + 1;
    while (i != j) {
      if (i < j) {
        if (i > n_) break;
        Adapter::add_assign(data_[i], sx);
        i += i & -i;
      } else {
        if (j > n_) break;
        Adapter::add_assign(data_[j], nx);
        j += j & -j;
      }
    }
  }

  void set(int p, const T& x) {
    assert(0 <= p && p < n_);
    add(p, (T)Adapter::sub_val((Storage)x, get_storage(p)));
  }

  T get(int p) const {
    assert(0 <= p && p < n_);
    return (T)get_storage(p);
  }

  int size() const { return n_; }

 private:
  using Adapter = internal::AbelianGroupAdapter<T>;
  using Storage = typename Adapter::Storage;

  Storage get_storage(int p) const {
    Storage s{};
    for (int i = p + 1; i > 0; i -= i & -i) Adapter::add_assign(s, data_[i]);
    return s;
  }

  int n_ = 0;
  std::vector<Storage> data_;
};

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_DUAL_FENWICK_TREE_HPP_
