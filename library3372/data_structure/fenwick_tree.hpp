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

  explicit ScalarImpl(const std::vector<T>& a) : n_((int)a.size()) {
    if (n_ == 0) return;
    data_.resize(n_ + 1);
    for (int i = 0; i < n_; ++i) data_[i + 1] = (Storage)a[i];
    for (int i = 1; i <= n_; ++i) {
      int j = i + (i & -i);
      if (j <= n_) add_assign(data_[j], data_[i]);
    }
  }

  void add(int p, const T& x) { add_storage(p, (Storage)x); }

  void set(int p, const T& x) {
    add_storage(p, sub_val((Storage)x, get_storage(p)));
  }

  T sum(int l, int r) const { return (T)sub_val(prefix_sum(r), prefix_sum(l)); }

  int max_right(int l, const T& x) const {
    if (l == n_) return n_;
    Storage base = (l > 0) ? prefix_sum(l) : Storage{};
    Storage s{};
    int r = 0;
    for (int d = 1 << (std::bit_width((unsigned int)n_) - 1); d > 0; d >>= 1) {
      if (r + d <= n_) {
        Storage next_s = add_val(s, data_[r + d]);
        if (r + d <= l || (T)sub_val(next_s, base) <= x) {
          r += d;
          s = std::move(next_s);
        }
      }
    }
    return r;
  }

  int min_left(int r, const T& x) const {
    if (r == 0) return 0;
    Storage s_r = prefix_sum(r);
    if ((T)s_r <= x) return 0;
    Storage s{};
    int l = 0;
    for (int d = 1 << (std::bit_width((unsigned int)r) - 1); d > 0; d >>= 1) {
      if (l + d < r) {
        Storage next_s = add_val(s, data_[l + d]);
        if (!((T)sub_val(s_r, next_s) <= x)) {
          l += d;
          s = std::move(next_s);
        }
      }
    }
    return l + 1;
  }

  int size() const { return n_; }

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
  alignas(32) static constexpr unsigned int suffix_32[8][8] = {
      {~0U, ~0U, ~0U, ~0U, ~0U, ~0U, ~0U, ~0U},
      {0U, ~0U, ~0U, ~0U, ~0U, ~0U, ~0U, ~0U},
      {0U, 0U, ~0U, ~0U, ~0U, ~0U, ~0U, ~0U},
      {0U, 0U, 0U, ~0U, ~0U, ~0U, ~0U, ~0U},
      {0U, 0U, 0U, 0U, ~0U, ~0U, ~0U, ~0U},
      {0U, 0U, 0U, 0U, 0U, ~0U, ~0U, ~0U},
      {0U, 0U, 0U, 0U, 0U, 0U, ~0U, ~0U},
      {0U, 0U, 0U, 0U, 0U, 0U, 0U, ~0U},
  };

  alignas(32) static constexpr unsigned long long suffix_64[8][8] = {
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

  explicit Avx2Impl(const std::vector<T>& a) : n_((int)a.size()) {
    if (n_ == 0) return;
    init_layout(n_);
    nodes_.resize(total_nodes_);

    int full_blocks = n_ >> log_b;
    for (int b = 0; b < full_blocks; ++b) {
      Storage total = prefix_sum_block<false>(&a[b << log_b], nodes_[b]);
      if (height_ > 1) {
        nodes_[level_offset_[1] + (b >> log_b)].v[b & mask] = total;
      }
    }
    int rem = n_ & mask;
    if (rem > 0) {
      Node tmp{};
      int base = full_blocks << log_b;
      for (int j = 0; j < rem; ++j) tmp.v[j] = (Storage)a[base + j];
      Storage total = prefix_sum_block<true>(tmp.v, nodes_[full_blocks]);
      if (height_ > 1) {
        nodes_[level_offset_[1] + (full_blocks >> log_b)]
            .v[full_blocks & mask] = total;
      }
    }

    for (int lvl = 1; lvl < height_; ++lvl) {
      int start = level_offset_[lvl];
      int end = (lvl + 1 < height_) ? level_offset_[lvl + 1] : total_nodes_;
      for (int b = start; b < end; ++b) {
        Storage total = prefix_sum_block<true>(nodes_[b].v, nodes_[b]);
        if (lvl + 1 < height_) {
          int idx = b - start;
          nodes_[level_offset_[lvl + 1] + (idx >> log_b)].v[idx & mask] = total;
        }
      }
    }
  }

  void add(int p, const T& x) { add_storage(p, (Storage)x); }

  void set(int p, const T& x) { add_storage(p, (Storage)x - get_storage(p)); }

  T sum(int l, int r) const {
    Storage sr = 0;
    Storage sl = 0;
    for (int lvl = 0; l != r; ++lvl) {
      int jr = r & mask;
      int jl = l & mask;
      r >>= log_b;
      l >>= log_b;
      if (jr > 0) sr += nodes_[level_offset_[lvl] + r].v[jr - 1];
      if (jl > 0) sl += nodes_[level_offset_[lvl] + l].v[jl - 1];
    }
    return (T)(sr - sl);
  }

  int max_right(int l, const T& x) const {
    if (l == n_) return n_;
    Storage base = (l > 0) ? prefix_sum(l) : 0;
    Storage s = 0;
    int node_idx = 0;
    long long pos = 0;
    int shift = (height_ - 1) * log_b;
    for (int lvl = height_ - 1; lvl >= 0; --lvl, shift -= log_b) {
      const Node& node = nodes_[level_offset_[lvl] + node_idx];
      int max_k = (int)std::min<long long>(block_size, (n_ - pos) >> shift);
      int k = 0;
      if (pos >= l) {
        k = first_match<false>(node, (Storage)x - (s - base), max_k);
      } else {
        int min_k = (int)std::min<long long>(block_size, (l - pos) >> shift);
        for (int step = block_size >> 1; step > 0; step >>= 1) {
          int cand = k + step;
          if (cand <= min_k ||
              (cand <= max_k && (T)(s + node.v[cand - 1] - base) <= x)) {
            k = cand;
          }
        }
      }
      if (k == block_size) return n_;
      if (k > 0) s += node.v[k - 1];
      pos += (long long)k << shift;
      if (pos == n_) return n_;
      node_idx = (node_idx << log_b) + k;
    }
    return (int)pos;
  }

  int min_left(int r, const T& x) const {
    if (r == 0) return 0;
    Storage s_r = prefix_sum(r);
    if ((T)s_r <= x) return 0;
    Storage s = 0;
    int node_idx = 0;
    long long pos = 0;
    int shift = (height_ - 1) * log_b;
    for (int lvl = height_ - 1; lvl >= 0; --lvl, shift -= log_b) {
      const Node& node = nodes_[level_offset_[lvl] + node_idx];
      int max_k =
          (int)std::min<long long>(block_size - 1, (r - 1 - pos) >> shift);
      int k = first_match<true>(node, (s_r - s) - (Storage)x, max_k);
      if (k > 0) s += node.v[k - 1];
      pos += (long long)k << shift;
      node_idx = (node_idx << log_b) + k;
    }
    return (int)pos + 1;
  }

  int size() const { return n_; }

 private:
  static constexpr int log_b = 3;
  static constexpr int block_size = 1 << log_b;
  static constexpr int mask = block_size - 1;

  struct alignas(sizeof(Storage) * block_size) Node {
    Storage v[block_size];
  };

  void init_layout(int n) {
    int cnt = (n >> log_b) + 1;
    height_ = 0;
    total_nodes_ = 0;
    while (true) {
      level_offset_[height_++] = total_nodes_;
      total_nodes_ += cnt;
      if (cnt == 1) break;
      cnt = (cnt + block_size - 1) >> log_b;
    }
  }

  static void add_suffix(Node& node, int j, Storage x) {
    if constexpr (sizeof(Storage) == 4) {
      __m256i vx = _mm256_set1_epi32((int)x);
      __m256i m = _mm256_load_si256((const __m256i*)Avx2Masks::suffix_32[j]);
      __m256i v = _mm256_load_si256((const __m256i*)node.v);
      v = _mm256_add_epi32(v, _mm256_and_si256(vx, m));
      _mm256_store_si256((__m256i*)node.v, v);
    } else {
      __m256i vx = _mm256_set1_epi64x((long long)x);
      __m256i m0 = _mm256_load_si256((const __m256i*)Avx2Masks::suffix_64[j]);
      __m256i m1 =
          _mm256_load_si256((const __m256i*)(Avx2Masks::suffix_64[j] + 4));
      __m256i v0 = _mm256_load_si256((const __m256i*)node.v);
      __m256i v1 = _mm256_load_si256((const __m256i*)(node.v + 4));
      v0 = _mm256_add_epi64(v0, _mm256_and_si256(vx, m0));
      v1 = _mm256_add_epi64(v1, _mm256_and_si256(vx, m1));
      _mm256_store_si256((__m256i*)node.v, v0);
      _mm256_store_si256((__m256i*)(node.v + 4), v1);
    }
  }

  template <bool IsGe>
  static int first_match(const Node& node, Storage bound, int max_k) {
    if constexpr (sizeof(Storage) == 4) {
      __m256i v = _mm256_load_si256((const __m256i*)node.v);
      __m256i vb = _mm256_set1_epi32((int)bound);
      if constexpr (internal::is_unsigned_integer_v<T>) {
        __m256i sign = _mm256_set1_epi32((int)0x80000000U);
        v = _mm256_xor_si256(v, sign);
        vb = _mm256_xor_si256(vb, sign);
      }
      __m256i cmp =
          IsGe ? _mm256_cmpgt_epi32(vb, v) : _mm256_cmpgt_epi32(v, vb);
      unsigned int m = (unsigned int)_mm256_movemask_epi8(cmp);
      if constexpr (IsGe) m = ~m;
      return std::countr_zero((unsigned long long)m | (1ULL << (max_k * 4))) >>
             2;
    } else {
      __m256i v0 = _mm256_load_si256((const __m256i*)node.v);
      __m256i v1 = _mm256_load_si256((const __m256i*)(node.v + 4));
      __m256i vb = _mm256_set1_epi64x((long long)bound);
      if constexpr (internal::is_unsigned_integer_v<T>) {
        __m256i sign = _mm256_set1_epi64x((long long)0x8000000000000000ULL);
        v0 = _mm256_xor_si256(v0, sign);
        v1 = _mm256_xor_si256(v1, sign);
        vb = _mm256_xor_si256(vb, sign);
      }
      __m256i cmp0 =
          IsGe ? _mm256_cmpgt_epi64(vb, v0) : _mm256_cmpgt_epi64(v0, vb);
      __m256i cmp1 =
          IsGe ? _mm256_cmpgt_epi64(vb, v1) : _mm256_cmpgt_epi64(v1, vb);
      unsigned int m0 =
          (unsigned int)_mm256_movemask_pd(_mm256_castsi256_pd(cmp0));
      unsigned int m1 =
          (unsigned int)_mm256_movemask_pd(_mm256_castsi256_pd(cmp1));
      unsigned int m = m0 | (m1 << 4);
      if constexpr (IsGe) m = (~m) & 0xFFU;
      return std::countr_zero(m | (1U << max_k));
    }
  }

  static __m256i prefix_sum_half64(__m256i v) {
    v = _mm256_add_epi64(v, _mm256_slli_si256(v, 8));
    __m256i lo = _mm256_permute4x64_epi64(v, 0x55);
    lo = _mm256_blend_epi32(lo, _mm256_setzero_si256(), 0x0F);
    return _mm256_add_epi64(v, lo);
  }

  template <bool Aligned, typename Ptr>
  static Storage prefix_sum_block(const Ptr* src, Node& dst) {
    if constexpr (sizeof(Storage) == 4) {
      __m256i v = Aligned ? _mm256_load_si256((const __m256i*)src)
                          : _mm256_loadu_si256((const __m256i*)src);
      v = _mm256_add_epi32(v, _mm256_slli_si256(v, 4));
      v = _mm256_add_epi32(v, _mm256_slli_si256(v, 8));
      __m256i s = _mm256_shuffle_epi32(v, 0xFF);
      __m256i carry = _mm256_permute2x128_si256(s, s, 0x08);
      v = _mm256_add_epi32(v, carry);
      _mm256_store_si256((__m256i*)dst.v, v);
      return dst.v[7];
    } else {
      __m256i v0 = Aligned ? _mm256_load_si256((const __m256i*)src)
                           : _mm256_loadu_si256((const __m256i*)src);
      __m256i v1 = Aligned ? _mm256_load_si256((const __m256i*)(src + 4))
                           : _mm256_loadu_si256((const __m256i*)(src + 4));
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
      int j = p & mask;
      p >>= log_b;
      add_suffix(nodes_[level_offset_[lvl] + p], j, x);
    }
  }

  Storage get_storage(int p) const {
    const Node& node = nodes_[p >> log_b];
    int j = p & mask;
    return j > 0 ? node.v[j] - node.v[j - 1] : node.v[0];
  }

  Storage prefix_sum(int r) const {
    Storage s = 0;
    for (int lvl = 0; r > 0; ++lvl) {
      int j = r & mask;
      r >>= log_b;
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

#ifdef __AVX2__
template <typename T, typename Storage>
using ImplType =
    std::conditional_t<internal::is_integer_v<Storage> &&
                           (sizeof(Storage) == 4 || sizeof(Storage) == 8),
                       Avx2Impl<T, Storage>, ScalarImpl<T, Storage>>;
#else
template <typename T, typename Storage> using ImplType = ScalarImpl<T, Storage>;
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

  void add(int p, const T& x) {
    assert(0 <= p && p < size());
    impl_.add(p, x);
  }

  void set(int p, const T& x) {
    assert(0 <= p && p < size());
    impl_.set(p, x);
  }

  T sum(int l, int r) const {
    assert(0 <= l && l <= r && r <= size());
    return impl_.sum(l, r);
  }

  int max_right(int l, const T& x) const
    requires internal::is_integer_v<T>
  {
    assert(0 <= l && l <= size());
    assert(T{} <= x);
    return impl_.max_right(l, x);
  }

  int min_left(int r, const T& x) const
    requires internal::is_integer_v<T>
  {
    assert(0 <= r && r <= size());
    assert(T{} <= x);
    return impl_.min_left(r, x);
  }

  int size() const { return impl_.size(); }

 private:
  using Storage = typename internal_fenwick_tree::StorageType<T>::type;
  internal_fenwick_tree::ImplType<T, Storage> impl_;
};

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_FENWICK_TREE_HPP_
