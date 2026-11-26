// Repository: Library3372
// File: library3372/data_structure/lazy_segment_tree.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/data_structure/lazy_segment_tree.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_LAZY_SEGMENT_TREE_HPP_
#define LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_LAZY_SEGMENT_TREE_HPP_

#include <algorithm>
#include <bit>
#include <cassert>
#include <concepts>
#include <type_traits>
#include <vector>

#include "library3372/internal/algebra/acted_monoid.hpp"

namespace library3372 {

// Bottom-up lazy segment tree for range-action and range-monoid queries
// https://en.wikipedia.org/wiki/Segment_tree
// https://cp-algorithms.com/data_structures/segment_tree.html
template <typename S, auto op, auto e, typename F, auto mapping,
          auto composition, auto id>
  requires internal::ActedMonoid<S, op, e, F, mapping, composition, id>
class LazySegmentTree {
 public:
  LazySegmentTree() = default;

  explicit LazySegmentTree(int n) : n_(n >= 0 ? n : 0) {
    assert(0 <= n && n <= (1 << 30));
    if (n_ == 0) return;
    size_ = (int)std::bit_ceil((unsigned int)n_);
    log_ = std::countr_zero((unsigned int)size_);
    data_.assign((unsigned long long)size_ * 2, SAdapter::e_storage());
    lazy_.assign((unsigned long long)size_, FAdapter::e_storage());
  }

  explicit LazySegmentTree(const std::vector<S>& a) {
    assert(a.size() <= (1ULL << 30));
    n_ = (int)a.size();
    if (n_ == 0) return;
    size_ = (int)std::bit_ceil((unsigned int)n_);
    log_ = std::countr_zero((unsigned int)size_);
    SStorage e_val = SAdapter::e_storage();
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
    lazy_.assign((unsigned long long)size_, FAdapter::e_storage());
  }

  void set(int p, const S& x) {
    assert(0 <= p && p < n_);
    unsigned int k = (unsigned int)(p + size_);
    for (int i = log_; i >= 1; --i) push(k >> i);
    data_[k] = SAdapter::to_storage(x);
    for (int i = 1; i <= log_; ++i) update(k >> i);
  }

  S get(int p) {
    assert(0 <= p && p < n_);
    unsigned int k = (unsigned int)(p + size_);
    for (int i = log_; i >= 1; --i) push(k >> i);
    return SAdapter::to_s(data_[k]);
  }

  S prod(int l, int r) {
    assert(0 <= l && l <= r && r <= n_);
    if (l == r) return SAdapter::call_e();
    unsigned int ul = (unsigned int)(l + size_);
    if (l + 1 == r) {
      for (int i = log_; i >= 1; --i) push(ul >> i);
      return SAdapter::to_s(data_[ul]);
    }
    unsigned int ur = (unsigned int)(r + size_);
    int tz_l = std::countr_zero(ul);
    int tz_r = std::countr_zero(ur);
    push_ancestors(ul, ur, tz_l, tz_r);
    int h = std::min(tz_l, std::bit_width(ur - ul) - 1);
    SStorage sm = data_[ul >> h];
    ul += 1U << h;
    while (ul < ur) {
      h = std::min(std::countr_zero(ul), std::bit_width(ur - ul) - 1);
      sm = SAdapter::op_storage(sm, data_[ul >> h]);
      ul += 1U << h;
    }
    return SAdapter::to_s(sm);
  }

  S all_prod() const {
    return n_ == 0 ? SAdapter::call_e() : SAdapter::to_s(data_[1]);
  }

  void apply(int p, const F& f) {
    assert(0 <= p && p < n_);
    unsigned int k = (unsigned int)(p + size_);
    for (int i = log_; i >= 1; --i) push(k >> i);
    data_[k] = Acted::mapping_storage(FAdapter::to_storage(f), data_[k]);
    for (int i = 1; i <= log_; ++i) update(k >> i);
  }

  void apply(int l, int r, const F& f) {
    assert(0 <= l && l <= r && r <= n_);
    if (l == r) return;
    if (l + 1 == r) {
      apply(l, f);
      return;
    }
    unsigned int ul = (unsigned int)(l + size_);
    unsigned int ur = (unsigned int)(r + size_);
    int tz_l = std::countr_zero(ul);
    int tz_r = std::countr_zero(ur);
    push_ancestors(ul, ur, tz_l, tz_r);
    FStorage f_val = FAdapter::to_storage(f);
    unsigned int cur_l = ul;
    unsigned int cur_r = ur;
    while (cur_l < cur_r) {
      int s = std::countr_zero(cur_l | cur_r);
      cur_l >>= s;
      cur_r >>= s;
      if (cur_l & 1) all_apply(cur_l++, f_val);
      if (cur_r & 1) all_apply(--cur_r, f_val);
      cur_l >>= 1;
      cur_r >>= 1;
    }
    update_ancestors(ul, ur, tz_l, tz_r);
  }

  template <typename Pred>
    requires std::predicate<Pred&, const S&>
  int max_right(int l, Pred pred) {
    assert(0 <= l && l <= n_);
    if constexpr (std::is_pointer_v<Pred>) assert(pred != nullptr);
    assert(pred(SAdapter::call_e()));
    if (l == n_) return n_;
    unsigned int ul = (unsigned int)(l + size_);
    for (int i = log_; i >= 1; --i) push(ul >> i);
    SStorage sm = SAdapter::e_storage();
    do {
      ul >>= std::countr_zero(ul);
      if (!pred(SAdapter::call_op(sm, data_[ul]))) {
        while (ul < (unsigned int)size_) {
          push(ul);
          ul <<= 1;
          if (pred(SAdapter::call_op(sm, data_[ul]))) {
            sm = SAdapter::op_storage(sm, data_[ul]);
            ++ul;
          }
        }
        return (int)ul - size_;
      }
      sm = SAdapter::op_storage(sm, data_[ul]);
      ++ul;
    } while (!std::has_single_bit(ul));
    return n_;
  }

  template <typename Pred>
    requires std::predicate<Pred&, const S&>
  int min_left(int r, Pred pred) {
    assert(0 <= r && r <= n_);
    if constexpr (std::is_pointer_v<Pred>) assert(pred != nullptr);
    assert(pred(SAdapter::call_e()));
    if (r == 0) return 0;
    unsigned int ur = (unsigned int)(r + size_);
    for (int i = log_; i >= 1; --i) push((ur - 1) >> i);
    SStorage sm = SAdapter::e_storage();
    do {
      --ur;
      ur >>= std::countr_one(ur);
      if (ur == 0) ur = 1;
      if (!pred(SAdapter::call_op(data_[ur], sm))) {
        while (ur < (unsigned int)size_) {
          push(ur);
          ur = (ur << 1) + 1;
          if (pred(SAdapter::call_op(data_[ur], sm))) {
            sm = SAdapter::op_storage(data_[ur], sm);
            --ur;
          }
        }
        return (int)ur + 1 - size_;
      }
      sm = SAdapter::op_storage(data_[ur], sm);
    } while (!std::has_single_bit(ur));
    return 0;
  }

  int size() const { return n_; }

 private:
  using Acted =
      internal::ActedMonoidAdapter<S, op, e, F, mapping, composition, id>;
  using SAdapter = typename Acted::SAdapter;
  using FAdapter = typename Acted::FAdapter;
  using SStorage = typename Acted::SStorage;
  using FStorage = typename Acted::FStorage;

  void update(unsigned int k) {
    data_[k] = SAdapter::op_storage(data_[k << 1], data_[(k << 1) | 1]);
  }

  void all_apply(unsigned int k, const FStorage& f) {
    data_[k] = Acted::mapping_storage(f, data_[k]);
    if (k < (unsigned int)size_) lazy_[k] = FAdapter::op_storage(f, lazy_[k]);
  }

  void push(unsigned int k) {
    all_apply(k << 1, lazy_[k]);
    all_apply((k << 1) | 1, lazy_[k]);
    lazy_[k] = FAdapter::e_storage();
  }

  void push_ancestors(unsigned int ul, unsigned int ur, int tz_l, int tz_r) {
    for (int i = log_; i >= 1; --i) {
      unsigned int p = ul >> i;
      unsigned int q = (ur - 1) >> i;
      bool pl = i > tz_l;
      bool pr = i > tz_r;
      if (pl) push(p);
      if (pr && (!pl || q != p)) push(q);
    }
  }

  void update_ancestors(unsigned int ul, unsigned int ur, int tz_l, int tz_r) {
    for (int i = 1; i <= log_; ++i) {
      unsigned int p = ul >> i;
      unsigned int q = (ur - 1) >> i;
      bool pl = i > tz_l;
      bool pr = i > tz_r;
      if (pl) update(p);
      if (pr && (!pl || q != p)) update(q);
    }
  }

  int n_ = 0;
  int size_ = 0;
  int log_ = 0;
  std::vector<SStorage> data_;
  std::vector<FStorage> lazy_;
};

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_LAZY_SEGMENT_TREE_HPP_
