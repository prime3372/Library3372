// Repository: Library3372
// File: library3372/data_structure/fenwick_tree.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/data_structure/fenwick_tree.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_FENWICK_TREE_HPP_
#define LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_FENWICK_TREE_HPP_

#include <bit>
#include <cassert>
#include <concepts>
#include <type_traits>
#include <utility>
#include <vector>

#include "library3372/internal/type_traits/is_integer.hpp"

namespace library3372 {

namespace internal_fenwick_tree {

template <typename T> struct StorageType {
  using type = T;
};

template <typename T>
  requires(internal::is_signed_integer_v<T> &&
           !std::is_same_v<std::remove_cv_t<T>, __int128>)
struct StorageType<T> {
  using type = std::make_unsigned_t<T>;
};

template <> struct StorageType<__int128> {
  using type = unsigned __int128;
};

}  // namespace internal_fenwick_tree

// Fenwick tree (Binary Indexed Tree)
// https://en.wikipedia.org/wiki/Fenwick_tree
template <typename T>
  requires(std::is_object_v<T> && !std::is_const_v<T> &&
           !std::is_volatile_v<T> && !std::is_same_v<T, bool>)
class FenwickTree {
 public:
  FenwickTree() = default;

  explicit FenwickTree(int n) : n_(n) {
    assert(n >= 0);
    if (n_ > 0) data_.assign(n_ + 1, Storage{});
  }

  explicit FenwickTree(const std::vector<T>& a) { build(a); }

  template <typename U>
    requires(!std::is_same_v<U, T> && std::constructible_from<T, const U&>)
  explicit FenwickTree(const std::vector<U>& a) {
    build(a);
  }

  int size() const { return n_; }

  void add(int p, const T& x) {
    assert(0 <= p && p < n_);
    add_storage(p, (Storage)x);
  }

  void set(int p, const T& x) {
    assert(0 <= p && p < n_);
    add_storage(p, sub_val((Storage)x, get_storage(p)));
  }

  T get(int p) const {
    assert(0 <= p && p < n_);
    return (T)get_storage(p);
  }

  T sum(int r) const {
    assert(0 <= r && r <= n_);
    return (T)prefix_sum(r);
  }

  T sum(int l, int r) const {
    assert(0 <= l && l <= r && r <= n_);
    return (T)sub_val(prefix_sum(r), prefix_sum(l));
  }

  template <typename F>
    requires std::predicate<F&, const T&>
  int max_right(F pred) const {
    assert(pred(T{}));
    if (n_ == 0) return 0;
    Storage s{};
    int r = 0;
    for (int d = 1 << (std::bit_width((unsigned int)n_) - 1); d > 0; d >>= 1) {
      if (r + d <= n_) {
        Storage next_s = add_val(s, data_[r + d]);
        if (pred((T)next_s)) {
          r += d;
          s = std::move(next_s);
        }
      }
    }
    return r;
  }

  template <typename F>
    requires std::predicate<F&, const T&>
  int max_right(int l, F pred) const {
    assert(0 <= l && l <= n_);
    if (l == 0) return max_right(std::move(pred));
    assert(pred(T{}));
    if (l == n_) return n_;
    Storage base = prefix_sum(l);
    Storage s{};
    int r = 0;
    for (int d = 1 << (std::bit_width((unsigned int)n_) - 1); d > 0; d >>= 1) {
      if (r + d <= n_) {
        Storage next_s = add_val(s, data_[r + d]);
        if (r + d <= l || pred((T)sub_val(next_s, base))) {
          r += d;
          s = std::move(next_s);
        }
      }
    }
    return r;
  }

  template <typename F>
    requires std::predicate<F&, const T&>
  int min_left(int r, F pred) const {
    assert(0 <= r && r <= n_);
    assert(pred(T{}));
    if (r == 0) return 0;
    Storage s_r = prefix_sum(r);
    if (pred((T)s_r)) return 0;
    Storage s{};
    int l = 0;
    for (int d = 1 << (std::bit_width((unsigned int)r) - 1); d > 0; d >>= 1) {
      if (l + d < r) {
        Storage next_s = add_val(s, data_[l + d]);
        if (!pred((T)sub_val(s_r, next_s))) {
          l += d;
          s = std::move(next_s);
        }
      }
    }
    return l + 1;
  }

 private:
  using Storage = typename internal_fenwick_tree::StorageType<T>::type;

  static void add_assign(Storage& a, const Storage& b) {
    if constexpr (sizeof(Storage) < sizeof(int) &&
                  internal::is_integer_v<Storage>) {
      a = (Storage)(a + b);
    } else {
      a += b;
    }
  }

  static Storage add_val(Storage a, const Storage& b) {
    add_assign(a, b);
    return a;
  }

  static Storage sub_val(Storage a, const Storage& b) {
    if constexpr (sizeof(Storage) < sizeof(int) &&
                  internal::is_integer_v<Storage>) {
      a = (Storage)(a - b);
    } else {
      a -= b;
    }
    return a;
  }

  template <typename U> void build(const std::vector<U>& a) {
    n_ = (int)a.size();
    if (n_ == 0) return;
    data_.resize(n_ + 1);
    for (int i = 0; i < n_; ++i) data_[i + 1] = (Storage)(T)a[i];
    for (int i = 1; i <= n_; ++i) {
      int j = i + (i & -i);
      if (j <= n_) add_assign(data_[j], data_[i]);
    }
  }

  void add_storage(int p, const Storage& x) {
    for (int i = p + 1; i <= n_; i += i & -i) add_assign(data_[i], x);
  }

  Storage get_storage(int p) const {
    int i = p + 1;
    int lca = i - (i & -i);
    Storage sub{};
    for (int j = i - 1; j > lca; j -= j & -j) add_assign(sub, data_[j]);
    return sub_val(data_[i], sub);
  }

  Storage prefix_sum(int r) const {
    Storage s{};
    for (int i = r; i > 0; i -= i & -i) add_assign(s, data_[i]);
    return s;
  }

  int n_ = 0;
  std::vector<Storage> data_;
};

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_FENWICK_TREE_HPP_
