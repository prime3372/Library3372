// Repository: Library3372
// File: library3372/data_structure/fenwick_tree.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/data_structure/fenwick_tree.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_FENWICK_TREE_HPP_
#define LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_FENWICK_TREE_HPP_

#include <algorithm>
#include <bit>
#include <cassert>
#include <concepts>
#include <type_traits>
#include <utility>
#include <vector>

#ifdef __AVX2__
#include <immintrin.h>
#endif

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

template <typename T, typename Storage> class ScalarImpl {
 public:
  ScalarImpl() = default;

  explicit ScalarImpl(int n) : n_(n) {
    if (n_ > 0) data_.assign(n_ + 1, Storage{});
  }

  template <typename U>
  explicit ScalarImpl(const std::vector<U>& a) : n_((int)a.size()) {
    if (n_ == 0) return;
    data_.resize(n_ + 1);
    for (int i = 0; i < n_; ++i) data_[i + 1] = (Storage)(T)a[i];
    for (int i = 1; i <= n_; ++i) {
      int j = i + (i & -i);
      if (j <= n_) add_assign(data_[j], data_[i]);
    }
  }

  int size() const { return n_; }

  void add(int p, const T& x) { add_storage(p, (Storage)x); }

  void set(int p, const T& x) {
    add_storage(p, sub_val((Storage)x, get_storage(p)));
  }

  T get(int p) const { return (T)get_storage(p); }

  T sum(int r) const { return (T)prefix_sum(r); }

  T sum(int l, int r) const { return (T)sub_val(prefix_sum(r), prefix_sum(l)); }

  template <typename F> int max_right(F pred) const {
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

  template <typename F> int max_right(int l, F pred) const {
    if (l == 0) return max_right(std::move(pred));
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

  template <typename F> int min_left(int r, F pred) const {
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

#ifdef __AVX2__

struct Avx2Masks {
  alignas(32) static constexpr unsigned int kSuffix32[8][8] = {
      {~0U, ~0U, ~0U, ~0U, ~0U, ~0U, ~0U, ~0U},
      {0U, ~0U, ~0U, ~0U, ~0U, ~0U, ~0U, ~0U},
      {0U, 0U, ~0U, ~0U, ~0U, ~0U, ~0U, ~0U},
      {0U, 0U, 0U, ~0U, ~0U, ~0U, ~0U, ~0U},
      {0U, 0U, 0U, 0U, ~0U, ~0U, ~0U, ~0U},
      {0U, 0U, 0U, 0U, 0U, ~0U, ~0U, ~0U},
      {0U, 0U, 0U, 0U, 0U, 0U, ~0U, ~0U},
      {0U, 0U, 0U, 0U, 0U, 0U, 0U, ~0U},
  };

  alignas(32) static constexpr unsigned long long kSuffix64[8][8] = {
      {~0ULL, ~0ULL, ~0ULL, ~0ULL, ~0ULL, ~0ULL, ~0ULL, ~0ULL},
      {0ULL, ~0ULL, ~0ULL, ~0ULL, ~0ULL, ~0ULL, ~0ULL, ~0ULL},
      {0ULL, 0ULL, ~0ULL, ~0ULL, ~0ULL, ~0ULL, ~0ULL, ~0ULL},
      {0ULL, 0ULL, 0ULL, ~0ULL, ~0ULL, ~0ULL, ~0ULL, ~0ULL},
      {0ULL, 0ULL, 0ULL, 0ULL, ~0ULL, ~0ULL, ~0ULL, ~0ULL},
      {0ULL, 0ULL, 0ULL, 0ULL, 0ULL, ~0ULL, ~0ULL, ~0ULL},
      {0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, ~0ULL, ~0ULL},
      {0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, ~0ULL},
  };
};

// 8-ary prefix-sum tree accelerated by AVX2
// https://en.algorithmica.org/hpc/data-structures/s-tree/
template <typename T, typename Storage> class Avx2Impl {
 public:
  Avx2Impl() = default;

  explicit Avx2Impl(int n) : n_(n) {
    if (n_ == 0) return;
    init_layout(n_);
    nodes_.assign(total_nodes_, Node{});
  }

  template <typename U>
  explicit Avx2Impl(const std::vector<U>& a) : n_((int)a.size()) {
    if (n_ == 0) return;
    init_layout(n_);
    nodes_.resize(total_nodes_);

    int lvl0_cnt = (height_ > 1) ? level_offset_[1] : total_nodes_;
    int full_blocks = n_ >> kLogB;
    for (int b = 0; b < full_blocks; ++b) {
      const int base = b << kLogB;
      Storage total;
      if constexpr (sizeof(U) == sizeof(Storage) && internal::is_integer_v<U>) {
        total = prefix_sum_unaligned(&a[base], nodes_[b]);
      } else {
        Node tmp;
        for (int j = 0; j < kBlockSize; ++j) tmp.v[j] = (Storage)(T)a[base + j];
        total = prefix_sum_node(tmp, nodes_[b]);
      }
      if (height_ > 1) {
        nodes_[level_offset_[1] + (b >> kLogB)].v[b & kMask] = total;
      }
    }
    if (full_blocks < lvl0_cnt) {
      int rem = n_ - (full_blocks << kLogB);
      Node tmp{};
      int base = full_blocks << kLogB;
      for (int j = 0; j < rem; ++j) tmp.v[j] = (Storage)(T)a[base + j];
      Storage total = prefix_sum_node(tmp, nodes_[full_blocks]);
      if (height_ > 1) {
        nodes_[level_offset_[1] + (full_blocks >> kLogB)]
            .v[full_blocks & kMask] = total;
      }
    }

    for (int lvl = 1; lvl < height_; ++lvl) {
      int start = level_offset_[lvl];
      int end = (lvl + 1 < height_) ? level_offset_[lvl + 1] : total_nodes_;
      for (int b = start; b < end; ++b) {
        Storage total = prefix_sum_node(nodes_[b], nodes_[b]);
        if (lvl + 1 < height_) {
          int idx = b - start;
          nodes_[level_offset_[lvl + 1] + (idx >> kLogB)].v[idx & kMask] =
              total;
        }
      }
    }
  }

  int size() const { return n_; }

  void add(int p, const T& x) { add_storage(p, (Storage)x); }

  void set(int p, const T& x) { add_storage(p, (Storage)x - get_storage(p)); }

  T get(int p) const { return (T)get_storage(p); }

  T sum(int r) const { return (T)prefix_sum(r); }

  T sum(int l, int r) const {
    Storage sr = 0;
    Storage sl = 0;
    for (int lvl = 0; l != r; ++lvl) {
      int jr = r & kMask;
      int jl = l & kMask;
      r >>= kLogB;
      l >>= kLogB;
      if (jr > 0) sr += nodes_[level_offset_[lvl] + r].v[jr - 1];
      if (jl > 0) sl += nodes_[level_offset_[lvl] + l].v[jl - 1];
    }
    return (T)(sr - sl);
  }

  template <typename F> int max_right(F pred) const {
    if (n_ == 0) return 0;
    Storage s = 0;
    int node_idx = 0;
    long long pos = 0;
    int shift = (height_ - 1) * kLogB;
    for (int lvl = height_ - 1; lvl >= 0; --lvl, shift -= kLogB) {
      const Node& node = nodes_[level_offset_[lvl] + node_idx];
      int max_k = (int)std::min<long long>(kBlockSize, (n_ - pos) >> shift);
      int k = 0;
      if (max_k == kBlockSize) {
        for (int step = kBlockSize >> 1; step > 0; step >>= 1) {
          if (pred((T)(s + node.v[k + step - 1]))) k += step;
        }
        if (k == kBlockSize) return n_;
      } else {
        for (int step = kBlockSize >> 1; step > 0; step >>= 1) {
          if (k + step <= max_k && pred((T)(s + node.v[k + step - 1]))) {
            k += step;
          }
        }
      }
      if (k > 0) s += node.v[k - 1];
      pos += (long long)k << shift;
      if (pos == n_) return n_;
      node_idx = (node_idx << kLogB) + k;
    }
    return (int)pos;
  }

  template <typename F> int max_right(int l, F pred) const {
    if (l == 0) return max_right(std::move(pred));
    if (l == n_) return n_;
    Storage base = prefix_sum(l);
    Storage s = 0;
    int node_idx = 0;
    long long pos = 0;
    int shift = (height_ - 1) * kLogB;
    for (int lvl = height_ - 1; lvl >= 0; --lvl, shift -= kLogB) {
      const Node& node = nodes_[level_offset_[lvl] + node_idx];
      int min_k = (pos < l)
                      ? (int)std::min<long long>(kBlockSize, (l - pos) >> shift)
                      : 0;
      int max_k = (int)std::min<long long>(kBlockSize, (n_ - pos) >> shift);
      int k = 0;
      if (min_k == 0 && max_k == kBlockSize) {
        for (int step = kBlockSize >> 1; step > 0; step >>= 1) {
          if (pred((T)(s + node.v[k + step - 1] - base))) k += step;
        }
        if (k == kBlockSize) return n_;
      } else {
        for (int step = kBlockSize >> 1; step > 0; step >>= 1) {
          int cand = k + step;
          if (cand <= min_k ||
              (cand <= max_k && pred((T)(s + node.v[cand - 1] - base)))) {
            k = cand;
          }
        }
        if (k == kBlockSize) return n_;
      }
      if (k > 0) s += node.v[k - 1];
      pos += (long long)k << shift;
      if (pos == n_) return n_;
      node_idx = (node_idx << kLogB) + k;
    }
    return (int)pos;
  }

  template <typename F> int min_left(int r, F pred) const {
    if (r == 0) return 0;
    Storage s_r = prefix_sum(r);
    if (pred((T)s_r)) return 0;
    Storage s = 0;
    int node_idx = 0;
    long long pos = 0;
    int shift = (height_ - 1) * kLogB;
    for (int lvl = height_ - 1; lvl >= 0; --lvl, shift -= kLogB) {
      const Node& node = nodes_[level_offset_[lvl] + node_idx];
      int max_k =
          (int)std::min<long long>(kBlockSize - 1, (r - 1 - pos) >> shift);
      int k = 0;
      if (max_k == kBlockSize - 1) {
        for (int step = kBlockSize >> 1; step > 0; step >>= 1) {
          if (!pred((T)(s_r - (s + node.v[k + step - 1])))) k += step;
        }
      } else {
        for (int step = kBlockSize >> 1; step > 0; step >>= 1) {
          if (k + step <= max_k &&
              !pred((T)(s_r - (s + node.v[k + step - 1])))) {
            k += step;
          }
        }
      }
      if (k > 0) s += node.v[k - 1];
      pos += (long long)k << shift;
      node_idx = (node_idx << kLogB) + k;
    }
    return (int)pos + 1;
  }

 private:
  static constexpr int kLogB = 3;
  static constexpr int kBlockSize = 1 << kLogB;
  static constexpr int kMask = kBlockSize - 1;

  struct alignas(sizeof(Storage) * kBlockSize) Node {
    Storage v[kBlockSize];
  };

  void init_layout(int n) {
    int cnt = (n + kBlockSize) >> kLogB;
    height_ = 0;
    total_nodes_ = 0;
    while (true) {
      level_offset_[height_++] = total_nodes_;
      total_nodes_ += cnt;
      if (cnt == 1) break;
      cnt = (cnt + kBlockSize - 1) >> kLogB;
    }
  }

  static void add_suffix(Node& node, int j, Storage x) {
    if constexpr (sizeof(Storage) == 4) {
      __m256i vx = _mm256_set1_epi32((int)x);
      __m256i m = _mm256_load_si256((const __m256i*)Avx2Masks::kSuffix32[j]);
      __m256i v = _mm256_load_si256((const __m256i*)node.v);
      v = _mm256_add_epi32(v, _mm256_and_si256(vx, m));
      _mm256_store_si256((__m256i*)node.v, v);
    } else {
      __m256i vx = _mm256_set1_epi64x((long long)x);
      __m256i m0 = _mm256_load_si256((const __m256i*)Avx2Masks::kSuffix64[j]);
      __m256i m1 =
          _mm256_load_si256((const __m256i*)(Avx2Masks::kSuffix64[j] + 4));
      __m256i v0 = _mm256_load_si256((const __m256i*)node.v);
      __m256i v1 = _mm256_load_si256((const __m256i*)(node.v + 4));
      v0 = _mm256_add_epi64(v0, _mm256_and_si256(vx, m0));
      v1 = _mm256_add_epi64(v1, _mm256_and_si256(vx, m1));
      _mm256_store_si256((__m256i*)node.v, v0);
      _mm256_store_si256((__m256i*)(node.v + 4), v1);
    }
  }

  static __m256i prefix_sum_half64(__m256i v) {
    v = _mm256_add_epi64(v, _mm256_slli_si256(v, 8));
    __m256i lo = _mm256_permute4x64_epi64(v, 0x55);
    lo = _mm256_blend_epi32(lo, _mm256_setzero_si256(), 0x0F);
    return _mm256_add_epi64(v, lo);
  }

  static Storage prefix_sum_node(const Node& src, Node& dst) {
    if constexpr (sizeof(Storage) == 4) {
      __m256i v = _mm256_load_si256((const __m256i*)src.v);
      v = _mm256_add_epi32(v, _mm256_slli_si256(v, 4));
      v = _mm256_add_epi32(v, _mm256_slli_si256(v, 8));
      __m256i s = _mm256_shuffle_epi32(v, 0xFF);
      __m256i carry = _mm256_permute2x128_si256(s, s, 0x08);
      v = _mm256_add_epi32(v, carry);
      _mm256_store_si256((__m256i*)dst.v, v);
      return dst.v[7];
    } else {
      __m256i v0 = _mm256_load_si256((const __m256i*)src.v);
      __m256i v1 = _mm256_load_si256((const __m256i*)(src.v + 4));
      v0 = prefix_sum_half64(v0);
      v1 = prefix_sum_half64(v1);
      __m256i carry = _mm256_permute4x64_epi64(v0, 0xFF);
      v1 = _mm256_add_epi64(v1, carry);
      _mm256_store_si256((__m256i*)dst.v, v0);
      _mm256_store_si256((__m256i*)(dst.v + 4), v1);
      return dst.v[7];
    }
  }

  template <typename U>
  static Storage prefix_sum_unaligned(const U* src, Node& dst) {
    if constexpr (sizeof(Storage) == 4) {
      __m256i v = _mm256_loadu_si256((const __m256i*)src);
      v = _mm256_add_epi32(v, _mm256_slli_si256(v, 4));
      v = _mm256_add_epi32(v, _mm256_slli_si256(v, 8));
      __m256i s = _mm256_shuffle_epi32(v, 0xFF);
      __m256i carry = _mm256_permute2x128_si256(s, s, 0x08);
      v = _mm256_add_epi32(v, carry);
      _mm256_store_si256((__m256i*)dst.v, v);
      return dst.v[7];
    } else {
      __m256i v0 = _mm256_loadu_si256((const __m256i*)src);
      __m256i v1 = _mm256_loadu_si256((const __m256i*)(src + 4));
      v0 = prefix_sum_half64(v0);
      v1 = prefix_sum_half64(v1);
      __m256i carry = _mm256_permute4x64_epi64(v0, 0xFF);
      v1 = _mm256_add_epi64(v1, carry);
      _mm256_store_si256((__m256i*)dst.v, v0);
      _mm256_store_si256((__m256i*)(dst.v + 4), v1);
      return dst.v[7];
    }
  }

  void add_storage(int p, Storage x) {
    for (int lvl = 0; lvl < height_; ++lvl) {
      int j = p & kMask;
      p >>= kLogB;
      add_suffix(nodes_[level_offset_[lvl] + p], j, x);
    }
  }

  Storage get_storage(int p) const {
    const Node& node = nodes_[p >> kLogB];
    int j = p & kMask;
    return j > 0 ? node.v[j] - node.v[j - 1] : node.v[0];
  }

  Storage prefix_sum(int r) const {
    Storage s = 0;
    for (int lvl = 0; r > 0; ++lvl) {
      int j = r & kMask;
      r >>= kLogB;
      if (j > 0) s += nodes_[level_offset_[lvl] + r].v[j - 1];
    }
    return s;
  }

  int n_ = 0;
  int height_ = 0;
  int total_nodes_ = 0;
  int level_offset_[12] = {};
  std::vector<Node> nodes_;
};

#endif  // __AVX2__

template <typename T, typename Storage> constexpr bool use_avx2() {
#ifdef __AVX2__
  return internal::is_integer_v<Storage> &&
         (sizeof(Storage) == 4 || sizeof(Storage) == 8);
#else
  return false;
#endif
}

template <typename T, typename Storage, bool UseAvx2> struct ImplSelector {
  using type = ScalarImpl<T, Storage>;
};

#ifdef __AVX2__
template <typename T, typename Storage> struct ImplSelector<T, Storage, true> {
  using type = Avx2Impl<T, Storage>;
};
#endif

}  // namespace internal_fenwick_tree

// Fenwick tree (Binary Indexed Tree)
// https://en.wikipedia.org/wiki/Fenwick_tree
template <typename T>
  requires(std::is_object_v<T> && !std::is_const_v<T> &&
           !std::is_volatile_v<T> && !std::is_same_v<T, bool>)
class FenwickTree {
 public:
  FenwickTree() = default;

  explicit FenwickTree(int n) : impl_(n >= 0 ? n : 0) { assert(n >= 0); }

  explicit FenwickTree(const std::vector<T>& a) : impl_(a) {}

  template <typename U>
    requires(!std::is_same_v<U, T> && std::constructible_from<T, const U&>)
  explicit FenwickTree(const std::vector<U>& a) : impl_(a) {}

  int size() const { return impl_.size(); }

  void add(int p, const T& x) {
    assert(0 <= p && p < size());
    impl_.add(p, x);
  }

  void set(int p, const T& x) {
    assert(0 <= p && p < size());
    impl_.set(p, x);
  }

  T get(int p) const {
    assert(0 <= p && p < size());
    return impl_.get(p);
  }

  T sum(int r) const {
    assert(0 <= r && r <= size());
    return impl_.sum(r);
  }

  T sum(int l, int r) const {
    assert(0 <= l && l <= r && r <= size());
    return impl_.sum(l, r);
  }

  template <typename F>
    requires std::predicate<F&, const T&>
  int max_right(F pred) const {
    assert(pred(T{}));
    return impl_.max_right(std::move(pred));
  }

  template <typename F>
    requires std::predicate<F&, const T&>
  int max_right(int l, F pred) const {
    assert(0 <= l && l <= size());
    assert(pred(T{}));
    return impl_.max_right(l, std::move(pred));
  }

  template <typename F>
    requires std::predicate<F&, const T&>
  int min_left(int r, F pred) const {
    assert(0 <= r && r <= size());
    assert(pred(T{}));
    return impl_.min_left(r, std::move(pred));
  }

 private:
  using Storage = typename internal_fenwick_tree::StorageType<T>::type;
  using Impl = typename internal_fenwick_tree::ImplSelector<
      T, Storage, internal_fenwick_tree::use_avx2<T, Storage>()>::type;

  Impl impl_;
};

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_FENWICK_TREE_HPP_
