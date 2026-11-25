// Repository: Library3372
// File: library3372/data_structure/segment_tree.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/data_structure/segment_tree.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_SEGMENT_TREE_HPP_
#define LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_SEGMENT_TREE_HPP_

#include <algorithm>
#include <bit>
#include <cassert>
#include <concepts>
#include <type_traits>
#include <vector>

#include "library3372/internal/algebra/monoid.hpp"

namespace library3372 {

// Bottom-up segment tree for point-update and range-monoid queries
// https://en.wikipedia.org/wiki/Segment_tree
// https://cp-algorithms.com/data_structures/segment_tree.html
template <typename S, auto op, auto e>
  requires internal::Monoid<S, op, e>
class SegmentTree {
 public:
  SegmentTree() = default;

  explicit SegmentTree(int n) : n_(n >= 0 ? n : 0) {
    assert(0 <= n && n <= (1 << 30));
    if (n_ == 0) return;
    size_ = (int)std::bit_ceil((unsigned int)n_);
    data_.assign((unsigned long long)size_ * 2, Adapter::e_storage());
  }

  explicit SegmentTree(const std::vector<S>& a) {
    assert(a.size() <= (1ULL << 30));
    n_ = (int)a.size();
    if (n_ == 0) return;
    size_ = (int)std::bit_ceil((unsigned int)n_);
    Storage e_val = Adapter::e_storage();
    data_.reserve((unsigned long long)size_ * 2);
    data_.resize(size_, e_val);
    data_.insert(data_.end(), a.begin(), a.end());
    if (n_ == size_) {
      for (unsigned int i = (unsigned int)size_ - 1; i >= 1; --i) update(i);
    } else {
      data_.resize((unsigned long long)size_ * 2, e_val);
      unsigned int l = (unsigned int)size_;
      unsigned int r = (unsigned int)(size_ + n_);
      while (l > 1) {
        l >>= 1;
        r = (r + 1) >> 1;
        for (unsigned int i = r; i-- > l;) update(i);
      }
    }
  }

  void set(int p, const S& x) {
    assert(0 <= p && p < n_);
    unsigned int k = (unsigned int)(p + size_);
    data_[k] = Adapter::to_storage(x);
    while (k > 1) {
      k >>= 1;
      update(k);
    }
  }

  S get(int p) const {
    assert(0 <= p && p < n_);
    return Adapter::to_s(data_[p + size_]);
  }

  S prod(int l, int r) const {
    assert(0 <= l && l <= r && r <= n_);
    if (l == r) return Adapter::call_e();
    if (l + 1 == r) return Adapter::to_s(data_[l + size_]);
    unsigned int ul = (unsigned int)(l + size_);
    unsigned int ur = (unsigned int)(r + size_);
    int h = std::min(std::countr_zero(ul), std::bit_width(ur - ul) - 1);
    Storage sm = data_[ul >> h];
    ul += 1U << h;
    while (ul < ur) {
      h = std::min(std::countr_zero(ul), std::bit_width(ur - ul) - 1);
      sm = Adapter::op_storage(sm, data_[ul >> h]);
      ul += 1U << h;
    }
    return Adapter::to_s(sm);
  }

  S all_prod() const {
    return n_ == 0 ? Adapter::call_e() : Adapter::to_s(data_[1]);
  }

  template <typename F>
    requires std::predicate<F&, const S&>
  int max_right(int l, F f) const {
    assert(0 <= l && l <= n_);
    if constexpr (std::is_pointer_v<F>) assert(f != nullptr);
    assert(f(Adapter::call_e()));
    if (l == n_) return n_;
    unsigned int ul = (unsigned int)(l + size_);
    Storage sm = Adapter::e_storage();
    do {
      ul >>= std::countr_zero(ul);
      if (!f(Adapter::call_op(sm, data_[ul]))) {
        while (ul < (unsigned int)size_) {
          ul <<= 1;
          if (f(Adapter::call_op(sm, data_[ul]))) {
            sm = Adapter::op_storage(sm, data_[ul]);
            ++ul;
          }
        }
        return (int)ul - size_;
      }
      sm = Adapter::op_storage(sm, data_[ul]);
      ++ul;
    } while (!std::has_single_bit(ul));
    return n_;
  }

  template <typename F>
    requires std::predicate<F&, const S&>
  int min_left(int r, F f) const {
    assert(0 <= r && r <= n_);
    if constexpr (std::is_pointer_v<F>) assert(f != nullptr);
    assert(f(Adapter::call_e()));
    if (r == 0) return 0;
    unsigned int ur = (unsigned int)(r + size_);
    Storage sm = Adapter::e_storage();
    do {
      --ur;
      ur >>= std::countr_one(ur);
      if (ur == 0) ur = 1;
      if (!f(Adapter::call_op(data_[ur], sm))) {
        while (ur < (unsigned int)size_) {
          ur = (ur << 1) + 1;
          if (f(Adapter::call_op(data_[ur], sm))) {
            sm = Adapter::op_storage(data_[ur], sm);
            --ur;
          }
        }
        return (int)ur + 1 - size_;
      }
      sm = Adapter::op_storage(data_[ur], sm);
    } while (!std::has_single_bit(ur));
    return 0;
  }

  int size() const { return n_; }

 private:
  using Adapter = internal::MonoidAdapter<S, op, e>;
  using Storage = typename Adapter::Storage;

  void update(unsigned int k) {
    data_[k] = Adapter::op_storage(data_[k << 1], data_[(k << 1) | 1]);
  }

  int n_ = 0;
  int size_ = 0;
  std::vector<Storage> data_;
};

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_SEGMENT_TREE_HPP_
