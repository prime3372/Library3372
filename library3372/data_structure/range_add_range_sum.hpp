// Repository: Library3372
// File: library3372/data_structure/range_add_range_sum.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/data_structure/range_add_range_sum.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_RANGE_ADD_RANGE_SUM_HPP_
#define LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_RANGE_ADD_RANGE_SUM_HPP_

#include <bit>
#include <cassert>
#include <utility>
#include <vector>

#include "library3372/internal/algebra/abelian_group.hpp"
#include "library3372/internal/type_traits/is_integer.hpp"

namespace library3372 {

// Range-add and range-sum query data structure using two Fenwick trees
// https://cp-algorithms.com/data_structures/fenwick.html#3-range-update-and-range-query
template <typename T>
  requires internal::IntScalableAbelianGroup<T>
class RangeAddRangeSum {
 public:
  RangeAddRangeSum() = default;

  explicit RangeAddRangeSum(int n) : n_(n >= 0 ? n : 0) {
    assert(n >= 0);
    if (n_ > 0) data_.assign(n_ + 1, Node{});
  }

  explicit RangeAddRangeSum(const std::vector<T>& a) : n_((int)a.size()) {
    if (n_ == 0) return;
    data_.resize(n_ + 1);
    Storage prev{};
    for (int i = 0; i < n_; ++i) {
      Storage cur = (Storage)a[i];
      Storage diff = Adapter::sub_val(cur, prev);
      data_[i + 1].d0 = diff;
      data_[i + 1].d1 = Adapter::mul_int(i, diff);
      prev = std::move(cur);
    }
    for (int i = 1; i <= n_; ++i) {
      int j = i + (i & -i);
      if (j <= n_) {
        Adapter::add_assign(data_[j].d0, data_[i].d0);
        Adapter::add_assign(data_[j].d1, data_[i].d1);
      }
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
    Storage x1_l = Adapter::mul_int(l, sx);
    int i = l + 1;
    if (r == n_) {
      for (; i <= n_; i += i & -i) {
        Adapter::add_assign(data_[i].d0, sx);
        Adapter::add_assign(data_[i].d1, x1_l);
      }
      return;
    }
    Storage nx = Adapter::neg_val(sx);
    Storage x1_r = Adapter::neg_val(Adapter::mul_int(r, sx));
    int j = r + 1;
    while (i != j) {
      if (i < j) {
        if (i > n_) break;
        Adapter::add_assign(data_[i].d0, sx);
        Adapter::add_assign(data_[i].d1, x1_l);
        i += i & -i;
      } else {
        if (j > n_) break;
        Adapter::add_assign(data_[j].d0, nx);
        Adapter::add_assign(data_[j].d1, x1_r);
        j += j & -j;
      }
    }
    if (i == j) {
      Storage x1_m = Adapter::add_val(x1_l, x1_r);
      for (; i <= n_; i += i & -i) Adapter::add_assign(data_[i].d1, x1_m);
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

  T sum(int l, int r) const {
    assert(0 <= l && l <= r && r <= n_);
    if (l == r) return T{};
    return (T)Adapter::sub_val(prefix_sum(r), prefix_sum(l));
  }

  int max_right(int l, const T& x) const
    requires internal::is_integer_v<T>
  {
    assert(0 <= l && l <= n_);
    assert(T{} <= x);
    if (l == n_) return n_;
    Storage base = (l > 0) ? prefix_sum(l) : Storage{};
    Storage s0{};
    Storage s1{};
    int r = 0;
    for (int d = 1 << (std::bit_width((unsigned int)n_) - 1); d > 0; d >>= 1) {
      if (r <= n_ - d) {
        Storage next_s0 = Adapter::add_val(s0, data_[r + d].d0);
        Storage next_s1 = Adapter::add_val(s1, data_[r + d].d1);
        Storage next_pref =
            Adapter::sub_val(Adapter::mul_int(r + d, next_s0), next_s1);
        if (r + d <= l || (T)Adapter::sub_val(next_pref, base) <= x) {
          r += d;
          s0 = std::move(next_s0);
          s1 = std::move(next_s1);
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
    Storage s0{};
    Storage s1{};
    int l = 0;
    for (int d = 1 << (std::bit_width((unsigned int)r) - 1); d > 0; d >>= 1) {
      if (l < r - d) {
        Storage next_s0 = Adapter::add_val(s0, data_[l + d].d0);
        Storage next_s1 = Adapter::add_val(s1, data_[l + d].d1);
        Storage next_pref =
            Adapter::sub_val(Adapter::mul_int(l + d, next_s0), next_s1);
        if (!((T)Adapter::sub_val(s_r, next_pref) <= x)) {
          l += d;
          s0 = std::move(next_s0);
          s1 = std::move(next_s1);
        }
      }
    }
    return l + 1;
  }

  int size() const { return n_; }

 private:
  using Adapter = internal::AbelianGroupAdapter<T>;
  using Storage = typename Adapter::Storage;

  struct Node {
    Storage d0{};
    Storage d1{};
  };

  Storage get_storage(int p) const {
    Storage s0{};
    for (int i = p + 1; i > 0; i -= i & -i)
      Adapter::add_assign(s0, data_[i].d0);
    return s0;
  }

  Storage prefix_sum(int r) const {
    Storage s0{};
    Storage s1{};
    for (int i = r; i > 0; i -= i & -i) {
      Adapter::add_assign(s0, data_[i].d0);
      Adapter::add_assign(s1, data_[i].d1);
    }
    return Adapter::sub_val(Adapter::mul_int(r, s0), s1);
  }

  int n_ = 0;
  std::vector<Node> data_;
};

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_RANGE_ADD_RANGE_SUM_HPP_
