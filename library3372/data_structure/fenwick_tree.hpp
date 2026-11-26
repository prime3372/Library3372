// Repository: Library3372
// File: library3372/data_structure/fenwick_tree.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/data_structure/fenwick_tree.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_FENWICK_TREE_HPP_
#define LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_FENWICK_TREE_HPP_

#include <bit>
#include <cassert>
#include <utility>
#include <vector>

#include "library3372/internal/algebra/abelian_group.hpp"
#include "library3372/internal/type_traits/is_integer.hpp"

namespace library3372 {

// Fenwick tree (Binary Indexed Tree)
// https://en.wikipedia.org/wiki/Fenwick_tree
template <typename T>
  requires internal::AbelianGroup<T>
class FenwickTree {
 public:
  FenwickTree() = default;

  explicit FenwickTree(int n) : n_(n >= 0 ? n : 0) {
    assert(n >= 0);
    if (n_ > 0) data_.assign(n_ + 1, Storage{});
  }

  explicit FenwickTree(const std::vector<T>& a) : n_((int)a.size()) {
    if (n_ == 0) return;
    data_.resize(n_ + 1);
    for (int i = 0; i < n_; ++i) data_[i + 1] = (Storage)a[i];
    for (int i = 1; i <= n_; ++i) {
      int j = i + (i & -i);
      if (j <= n_) Adapter::add_assign(data_[j], data_[i]);
    }
  }

  void add(int p, const T& x) {
    assert(0 <= p && p < n_);
    add_storage(p, (Storage)x);
  }

  void set(int p, const T& x) {
    assert(0 <= p && p < n_);
    add_storage(p, Adapter::sub_val((Storage)x, get_storage(p)));
  }

  T get(int p) const {
    assert(0 <= p && p < n_);
    return (T)get_storage(p);
  }

  T sum(int l, int r) const {
    assert(0 <= l && l <= r && r <= n_);
    Storage s{};
    while (r > l) {
      Adapter::add_assign(s, data_[r]);
      r -= r & -r;
    }
    while (l > r) {
      Adapter::sub_assign(s, data_[l]);
      l -= l & -l;
    }
    return (T)s;
  }

  int max_right(int l, const T& x) const
    requires internal::is_integer_v<T>
  {
    assert(0 <= l && l <= n_);
    assert(T{} <= x);
    if (l == n_) return n_;
    Storage base = (l > 0) ? prefix_sum(l) : Storage{};
    Storage s{};
    int r = 0;
    for (int d = 1 << (std::bit_width((unsigned int)n_) - 1); d > 0; d >>= 1) {
      if (r + d <= n_) {
        Storage next_s = Adapter::add_val(s, data_[r + d]);
        if (r + d <= l || (T)Adapter::sub_val(next_s, base) <= x) {
          r += d;
          s = std::move(next_s);
        }
      }
    }
    return r;
  }

  int min_left(int r, const T& x) const
    requires internal::is_integer_v<T>
  {
    assert(0 <= r && r <= n_);
    assert(T{} <= x);
    if (r == 0) return 0;
    Storage s_r = prefix_sum(r);
    if ((T)s_r <= x) return 0;
    Storage s{};
    int l = 0;
    for (int d = 1 << (std::bit_width((unsigned int)r) - 1); d > 0; d >>= 1) {
      if (l + d < r) {
        Storage next_s = Adapter::add_val(s, data_[l + d]);
        if (!((T)Adapter::sub_val(s_r, next_s) <= x)) {
          l += d;
          s = std::move(next_s);
        }
      }
    }
    return l + 1;
  }

  int size() const { return n_; }

 private:
  using Adapter = internal::AbelianGroupAdapter<T>;
  using Storage = typename Adapter::Storage;

  void add_storage(int p, const Storage& x) {
    for (int i = p + 1; i <= n_; i += i & -i) Adapter::add_assign(data_[i], x);
  }

  Storage get_storage(int p) const {
    int i = p + 1;
    int lca = i - (i & -i);
    Storage sub{};
    for (int j = i - 1; j > lca; j -= j & -j)
      Adapter::add_assign(sub, data_[j]);
    return Adapter::sub_val(data_[i], sub);
  }

  Storage prefix_sum(int r) const {
    Storage s{};
    for (int i = r; i > 0; i -= i & -i) Adapter::add_assign(s, data_[i]);
    return s;
  }

  int n_ = 0;
  std::vector<Storage> data_;
};

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_FENWICK_TREE_HPP_
