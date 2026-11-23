// Repository: Library3372
// File: library3372/string/suffix_array.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/string/suffix_array.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_STRING_SUFFIX_ARRAY_HPP_
#define LIBRARY3372_LIBRARY3372_STRING_SUFFIX_ARRAY_HPP_

#ifdef __AVX2__
#include <immintrin.h>
#endif

#include <algorithm>
#include <bit>
#include <cassert>
#include <concepts>
#include <cstring>
#include <functional>
#include <numeric>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "library3372/internal/type_traits/is_integer.hpp"

namespace library3372 {
namespace internal_suffix_array {

inline constexpr int threshold_naive = 32;
inline constexpr int tag_bit = (int)0x80000000U;
inline constexpr int lms_bit = 0x40000000;

template <typename Seq, typename Compare = std::less<>>
inline void sa_naive(const Seq& s, int n, int* sa,
                     Compare compare = Compare{}) {
  for (int i = 0; i < n; ++i) sa[i] = i;
  auto cmp = [&](int l, int r) {
    int limit = n - (l > r ? l : r);
    for (int k = 0; k < limit; ++k) {
      if (compare(s[l + k], s[r + k])) return true;
      if (compare(s[r + k], s[l + k])) return false;
    }
    return l > r;
  };
  for (int i = 1; i < n; ++i) {
    int x = sa[i];
    int j = i;
    while (j > 0 && cmp(x, sa[j - 1])) {
      sa[j] = sa[j - 1];
      --j;
    }
    sa[j] = x;
  }
}

#ifdef __AVX2__
struct CompressTable {
  unsigned char idx[256][8];
  constexpr CompressTable() : idx{} {
    for (int mask = 0; mask < 256; ++mask) {
      int k = 0;
      for (int b = 0; b < 8; ++b) {
        if ((mask >> b) & 1) idx[mask][k++] = (unsigned char)b;
      }
    }
  }
};

inline constexpr CompressTable compress_table{};

inline void set_buckets(const int* cnt, int* bkt, int upper, bool end) {
  if (upper < 64) {
    int sum = 0;
    if (end) {
      for (int i = 0; i <= upper; ++i) {
        sum += cnt[i];
        bkt[i] = sum;
      }
    } else {
      for (int i = 0; i <= upper; ++i) {
        bkt[i] = sum;
        sum += cnt[i];
      }
    }
    return;
  }
  int n = upper + 1;
  int i = 0;
  __m256i carry = _mm256_setzero_si256();
  for (; i + 8 <= n; i += 8) {
    __m256i x = _mm256_loadu_si256((const __m256i*)(cnt + i));
    __m256i s = _mm256_add_epi32(x, _mm256_slli_si256(x, 4));
    s = _mm256_add_epi32(s, _mm256_slli_si256(s, 8));
    __m256i lo_last = _mm256_permute2x128_si256(s, s, 0x08);
    lo_last = _mm256_shuffle_epi32(lo_last, 0xFF);
    s = _mm256_add_epi32(s, lo_last);
    __m256i inc = _mm256_add_epi32(s, carry);
    __m256i res = end ? inc : _mm256_sub_epi32(inc, x);
    _mm256_storeu_si256((__m256i*)(bkt + i), res);
    __m256i hi = _mm256_permute2x128_si256(inc, inc, 0x11);
    carry = _mm256_shuffle_epi32(hi, 0xFF);
  }
  int sum = _mm256_cvtsi256_si32(carry);
  if (end) {
    for (; i < n; ++i) {
      sum += cnt[i];
      bkt[i] = sum;
    }
  } else {
    for (; i < n; ++i) {
      bkt[i] = sum;
      sum += cnt[i];
    }
  }
}

template <typename CharT, typename FBlock, typename FTail>
inline void scan_lms_blocks_avx2(const CharT* s, int n, FBlock&& fn_block,
                                 FTail&& fn_tail) {
  int i = n - 2;
  unsigned int prev_s = 0;
  if constexpr (sizeof(CharT) == 1) {
    if (i >= 31) {
      const __m256i bias = _mm256_set1_epi8((char)0x80);
      const __m256i rev16 = _mm256_setr_epi8(
          15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 15, 14, 13, 12,
          11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);
      for (; i >= 31; i -= 32) {
        int base = i - 31;
        __m256i v_cur = _mm256_loadu_si256((const __m256i*)(s + base));
        __m256i v_next = _mm256_loadu_si256((const __m256i*)(s + base + 1));
        __m256i v_eq = _mm256_cmpeq_epi8(v_cur, v_next);
        __m256i v_gt = _mm256_cmpgt_epi8(_mm256_xor_si256(v_next, bias),
                                         _mm256_xor_si256(v_cur, bias));
        v_eq = _mm256_shuffle_epi8(_mm256_permute2x128_si256(v_eq, v_eq, 0x01),
                                   rev16);
        v_gt = _mm256_shuffle_epi8(_mm256_permute2x128_si256(v_gt, v_gt, 0x01),
                                   rev16);
        unsigned int eq = (unsigned int)_mm256_movemask_epi8(v_eq);
        unsigned int gt = (unsigned int)_mm256_movemask_epi8(v_gt);
        // Carry propagation propagates S-type from right (LSB) to left (MSB).
        unsigned int sum = eq + ((gt << 1) | prev_s);
        unsigned int s_mask = ((~sum) & eq) | gt;
        unsigned int lms_mask = (~s_mask) & ((s_mask << 1) | prev_s);
        prev_s = s_mask >> 31;
        fn_block(base + 32, lms_mask);
      }
    }
  } else if constexpr (std::is_same_v<CharT, int>) {
    if (i >= 7) {
      const __m256i rev8 = _mm256_setr_epi32(7, 6, 5, 4, 3, 2, 1, 0);
      for (; i >= 7; i -= 8) {
        int base = i - 7;
        __m256i v_cur = _mm256_loadu_si256((const __m256i*)(s + base));
        __m256i v_next = _mm256_loadu_si256((const __m256i*)(s + base + 1));
        __m256i v_eq = _mm256_permutevar8x32_epi32(
            _mm256_cmpeq_epi32(v_cur, v_next), rev8);
        __m256i v_gt = _mm256_permutevar8x32_epi32(
            _mm256_cmpgt_epi32(v_next, v_cur), rev8);
        unsigned int eq = (unsigned int)_mm256_movemask_ps((__m256)v_eq);
        unsigned int gt = (unsigned int)_mm256_movemask_ps((__m256)v_gt);
        unsigned int sum = eq + ((gt << 1) | prev_s);
        unsigned int s_mask = (((~sum) & eq) | gt) & 0xFFU;
        unsigned int lms_mask = ((~s_mask) & ((s_mask << 1) | prev_s)) & 0xFFU;
        prev_s = s_mask >> 7;
        fn_block(base + 8, lms_mask);
      }
    }
  }
  int ps = (int)prev_s;
  int next_c = (int)s[i + 1];
  for (; i >= 0; --i) {
    int c = (int)s[i];
    int cur_s = (c < next_c) | ((c == next_c) & ps);
    int is_lms = (~cur_s) & ps;
    fn_tail(i + 1, is_lms);
    ps = cur_s;
    next_c = c;
  }
}

template <typename CharT>
inline void count_symbols_and_lms(const CharT* s, int n, int upper, int* cnt,
                                  int& m, int& last_lms) {
  if constexpr (sizeof(CharT) == 1) {
    int cnt4[4][256] = {};
    int i = 0;
    for (; i + 3 < n; i += 4) {
      ++cnt4[0][s[i]];
      ++cnt4[1][s[i + 1]];
      ++cnt4[2][s[i + 2]];
      ++cnt4[3][s[i + 3]];
    }
    for (; i < n; ++i) ++cnt[s[i]];
    for (int c = 0; c <= upper; ++c) {
      cnt[c] += cnt4[0][c] + cnt4[1][c] + cnt4[2][c] + cnt4[3][c];
    }
  } else {
    for (int i = 0; i < n; ++i) ++cnt[s[i]];
  }
  m = 0;
  last_lms = -1;
  scan_lms_blocks_avx2(
      s, n,
      [&](int end_pos, unsigned int lms_mask) {
        if (lms_mask) {
          m += std::popcount(lms_mask);
          last_lms = end_pos - (31 - std::countl_zero(lms_mask));
        }
      },
      [&](int pos, int is_lms) {
        m += is_lms;
        last_lms = is_lms ? pos : last_lms;
      });
}

template <typename CharT, typename F>
inline void for_each_lms_pos(const CharT* s, int n, F&& fn) {
  scan_lms_blocks_avx2(
      s, n,
      [&](int end_pos, unsigned int lms_mask) {
        while (lms_mask) {
          fn(end_pos - std::countr_zero(lms_mask));
          lms_mask &= lms_mask - 1;
        }
      },
      [&](int pos, int is_lms) {
        if (is_lms) fn(pos);
      });
}

inline void compact_lms(int* sa, int n) {
  int k = 0;
  int i = 0;
  const __m256i clear_lms = _mm256_set1_epi32(~lms_bit);
  for (; i + 8 <= n && k + 8 <= n; i += 8) {
    __m256i v = _mm256_loadu_si256((const __m256i*)(sa + i));
    unsigned int mask =
        (unsigned int)_mm256_movemask_ps((__m256)_mm256_slli_epi32(v, 1));
    __m128i bytes = _mm_loadl_epi64((const __m128i*)compress_table.idx[mask]);
    __m256i perm = _mm256_cvtepu8_epi32(bytes);
    __m256i packed =
        _mm256_permutevar8x32_epi32(_mm256_and_si256(v, clear_lms), perm);
    _mm256_storeu_si256((__m256i*)(sa + k), packed);
    k += std::popcount(mask);
  }
  for (; i < n; ++i) {
    int v = sa[i];
    sa[k] = v & ~lms_bit;
    k += (v >> 30) & 1;
  }
}

inline void compact_rec_s(int* sa, int m, int n) {
  int* rec_s = sa + m;
  int j = 0;
  int i = m;
  for (; i + 8 <= n; i += 8) {
    __m256i v = _mm256_loadu_si256((const __m256i*)(sa + i));
    unsigned int mask = (~(unsigned int)_mm256_movemask_ps((__m256)v)) & 0xFFU;
    __m128i bytes = _mm_loadl_epi64((const __m128i*)compress_table.idx[mask]);
    __m256i perm = _mm256_cvtepu8_epi32(bytes);
    __m256i packed = _mm256_permutevar8x32_epi32(v, perm);
    _mm256_storeu_si256((__m256i*)(rec_s + j), packed);
    j += std::popcount(mask);
  }
  for (; i < n; ++i) {
    int v = sa[i];
    rec_s[j] = v;
    j += (v >= 0);
  }
}

template <typename CharT>
inline bool lms_substr_diff(const CharT* s, int n, int a, int b, int len) {
  if constexpr (sizeof(CharT) == 1) {
    if (len <= 16 && a + 16 <= n && b + 16 <= n) {
      __m128i va = _mm_loadu_si128((const __m128i*)(s + a));
      __m128i vb = _mm_loadu_si128((const __m128i*)(s + b));
      unsigned int eq = (unsigned int)_mm_movemask_epi8(_mm_cmpeq_epi8(va, vb));
      return ((~eq) & ((1U << len) - 1U)) != 0;
    }
    if (len > 16) {
      return std::memcmp(s + a, s + b, (unsigned long)len) != 0;
    }
  } else {
    if (len <= 8 && a + 8 <= n && b + 8 <= n) {
      __m256i va = _mm256_loadu_si256((const __m256i*)(s + a));
      __m256i vb = _mm256_loadu_si256((const __m256i*)(s + b));
      unsigned int eq =
          (unsigned int)_mm256_movemask_ps((__m256)_mm256_cmpeq_epi32(va, vb));
      return ((~eq) & ((1U << len) - 1U)) != 0;
    }
    if (len > 8) {
      return std::memcmp(s + a, s + b, (unsigned long)len * sizeof(int)) != 0;
    }
  }
  for (int d = 0; d < len; ++d) {
    if (s[a + d] != s[b + d]) return true;
  }
  return false;
}
#else
inline void set_buckets(const int* cnt, int* bkt, int upper, bool end) {
  int sum = 0;
  if (end) {
    for (int i = 0; i <= upper; ++i) {
      sum += cnt[i];
      bkt[i] = sum;
    }
  } else {
    for (int i = 0; i <= upper; ++i) {
      bkt[i] = sum;
      sum += cnt[i];
    }
  }
}

template <typename CharT, typename F>
inline void for_each_lms_reverse(const CharT* s, int n, F&& fn) {
  int prev_s = 0;
  int next_c = (int)s[n - 1];
  for (int i = n - 2; i >= 0; --i) {
    int c = (int)s[i];
    int cur_s = (c < next_c) | ((c == next_c) & prev_s);
    int is_lms = (~cur_s) & prev_s;
    fn(i + 1, is_lms);
    prev_s = cur_s;
    next_c = c;
  }
}

template <typename CharT>
inline void count_symbols_and_lms(const CharT* s, int n, int, int* cnt, int& m,
                                  int& last_lms) {
  m = 0;
  last_lms = -1;
  ++cnt[(int)s[n - 1]];
  for_each_lms_reverse(s, n, [&](int pos, int is_lms) {
    ++cnt[(int)s[pos - 1]];
    m += is_lms;
    last_lms = is_lms ? pos : last_lms;
  });
}

template <typename CharT, typename F>
inline void for_each_lms_pos(const CharT* s, int n, F&& fn) {
  for_each_lms_reverse(s, n, [&](int pos, int is_lms) {
    if (is_lms) fn(pos);
  });
}

inline void compact_lms(int* sa, int n) {
  int k = 0;
  for (int i = 0; i < n; ++i) {
    int v = sa[i];
    sa[k] = v & ~lms_bit;
    k += (v >> 30) & 1;
  }
}

inline void compact_rec_s(int* sa, int m, int n) {
  int* rec_s = sa + m;
  int j = 0;
  for (int i = m; i < n; ++i) {
    int v = sa[i];
    rec_s[j] = v;
    j += (v >= 0);
  }
}

template <typename CharT>
inline bool lms_substr_diff(const CharT* s, int, int a, int b, int len) {
  if constexpr (sizeof(CharT) == 1) {
    if (len > 16) {
      return std::memcmp(s + a, s + b, (unsigned long)len) != 0;
    }
  }
  for (int d = 0; d < len; ++d) {
    if (s[a + d] != s[b + d]) return true;
  }
  return false;
}
#endif

template <typename CharT>
inline void induce_l(const CharT* s, int n, const int* cnt, int* bkt, int upper,
                     int* sa) {
  set_buckets(cnt, bkt, upper, false);
  int c_last = (int)s[n - 1];
  sa[bkt[c_last]++] = ((int)s[n - 2] < c_last) ? ((n - 1) | tag_bit) : (n - 1);
  int dummy = 0;
  for (int i = 0; i < n; ++i) {
    int v = sa[i];
    if (v > 0) {
      // Initial LMS entries carry lms_bit so they can be cleared without a
      // separate L/S array; L-type predecessors are tagged if u-1 is S-type.
      int is_lms = v >> 30;
      int* clr = is_lms ? &sa[i] : &dummy;
      *clr = -1;
      v &= ~lms_bit;
      int u = v - 1;
      int cu = (int)s[u];
      sa[bkt[cu]++] = (u > 0 && (int)s[u - 1] < cu) ? (u | tag_bit) : u;
    }
  }
}

template <bool mark_lms, typename CharT>
inline void induce_s(const CharT* s, int n, const int* cnt, int* bkt, int upper,
                     int* sa) {
  set_buckets(cnt, bkt, upper, true);
  for (int i = n - 1; i >= 0; --i) {
    int v = sa[i];
    if (v < -1) {
      v &= ~tag_bit;
      sa[i] = v;
      int u = v - 1;
      int cu = (int)s[u];
      if constexpr (mark_lms) {
        if (u > 0) {
          sa[--bkt[cu]] = ((int)s[u - 1] <= cu) ? (u | tag_bit) : (u | lms_bit);
        } else {
          sa[--bkt[cu]] = 0;
        }
      } else {
        sa[--bkt[cu]] = (u > 0 && (int)s[u - 1] <= cu) ? (u | tag_bit) : u;
      }
    }
  }
}

// SA-IS (Suffix Array - Induced Sorting)
// https://en.wikipedia.org/wiki/Suffix_array
template <typename CharT>
void sa_is_core(const CharT* s, int n, int upper, int* sa,
                std::vector<int>& cnt_pool, int cnt_offset,
                std::vector<int>& bkt_vec) {
  if (n == 0) return;
  if (n == 1) {
    sa[0] = 0;
    return;
  }
  if (n == 2) {
    if (s[0] < s[1]) {
      sa[0] = 0;
      sa[1] = 1;
    } else {
      sa[0] = 1;
      sa[1] = 0;
    }
    return;
  }
  if (n <= threshold_naive) {
    sa_naive(s, n, sa);
    return;
  }

  if ((int)cnt_pool.size() < cnt_offset + upper + 1) {
    cnt_pool.resize(cnt_offset + upper + 1);
  }
  if ((int)bkt_vec.size() <= upper) bkt_vec.resize(upper + 1);

  int* cnt = cnt_pool.data() + cnt_offset;
  int* bkt = bkt_vec.data();
  std::fill(cnt, cnt + upper + 1, 0);

  int m = 0;
  int last_lms = -1;
  count_symbols_and_lms(s, n, upper, cnt, m, last_lms);
  assert(0 <= m && m <= n);

  std::memset(sa, 0xff, (unsigned long)n * sizeof(int));

  if (m >= 1) {
    set_buckets(cnt, bkt, upper, true);
    if (m == 1) {
      sa[--bkt[(int)s[last_lms]]] = last_lms | lms_bit;
    } else {
      for_each_lms_pos(
          s, n, [&](int pos) { sa[--bkt[(int)s[pos]]] = pos | lms_bit; });
    }
  }

  induce_l(s, n, cnt, bkt, upper, sa);
  if (m <= 1) {
    induce_s<false>(s, n, cnt, bkt, upper, sa);
    return;
  }
  induce_s<true>(s, n, cnt, bkt, upper, sa);

  compact_lms(sa, n);

  // No two adjacent indices can be LMS, so (p >> 1) uniquely maps every LMS
  // position into sa[m .. n - 1] without an external lms_map array.
  std::memset(sa + m, 0xff, (unsigned long)(n - m) * sizeof(int));
  {
    int prev_lms = n;
    for_each_lms_pos(s, n, [&](int p) {
      // Sentinel-terminated last LMS gets length 1 (non-last LMS have len >= 3).
      sa[m + (p >> 1)] = (prev_lms == n) ? 1 : (prev_lms - p + 1);
      prev_lms = p;
    });
  }

  int rec_upper = 0;
  {
    int prev_p = sa[0];
    int prev_len = sa[m + (prev_p >> 1)];
    sa[m + (prev_p >> 1)] = 0;
    for (int i = 1; i < m; ++i) {
      int p = sa[i];
      int len = sa[m + (p >> 1)];
      bool diff = (len != prev_len) || lms_substr_diff(s, n, prev_p, p, len);
      if (diff) {
        ++rec_upper;
        prev_p = p;
        prev_len = len;
      }
      sa[m + (p >> 1)] = rec_upper;
    }
  }

  if (rec_upper + 1 < m) {
    int* rec_s = sa + m;
    compact_rec_s(sa, m, n);
    sa_is_core<int>(rec_s, m, rec_upper, sa, cnt_pool, cnt_offset + upper + 1,
                    bkt_vec);
    int j = m;
    for_each_lms_pos(s, n, [&](int p) { sa[m + (--j)] = p; });
    for (int i = 0; i < m; ++i) sa[i] = sa[m + sa[i]];
    cnt = cnt_pool.data() + cnt_offset;
    bkt = bkt_vec.data();
  }

  std::memset(sa + m, 0xff, (unsigned long)(n - m) * sizeof(int));
  set_buckets(cnt, bkt, upper, true);
  for (int i = m - 1; i >= 0; --i) {
    int p = sa[i];
    sa[i] = -1;
    sa[--bkt[(int)s[p]]] = p | lms_bit;
  }

  induce_l(s, n, cnt, bkt, upper, sa);
  induce_s<false>(s, n, cnt, bkt, upper, sa);
}

template <typename U> inline int bit_width_u(U x) {
  if constexpr (sizeof(U) <= sizeof(unsigned long long)) {
    return std::bit_width(x);
  } else {
    unsigned long long hi = (unsigned long long)(x >> 64);
    if (hi != 0) return 64 + std::bit_width(hi);
    return std::bit_width((unsigned long long)x);
  }
}

template <typename T, typename U>
std::vector<int> suffix_array_radix(const std::vector<T>& s, T min_v, U range) {
  int n = (int)s.size();
  constexpr int bits = 10;
  constexpr int radix = 1 << bits;
  constexpr unsigned int mask = radix - 1;
  int total_bits = bit_width_u(range);
  int passes = (total_bits + bits - 1) / bits;

  struct Item {
    U key;
    int idx;
  };
  std::vector<Item> buf_a(n);
  std::vector<Item> buf_b(n);
  std::vector<int> counts(passes * radix, 0);

  for (int i = 0; i < n; ++i) {
    U k = (U)s[i] - (U)min_v;
    buf_a[i] = {k, i};
    for (int p = 0; p < passes; ++p) {
      ++counts[p * radix + (int)((unsigned int)(k >> (p * bits)) & mask)];
    }
  }
  for (int p = 0; p < passes; ++p) {
    int* c = counts.data() + p * radix;
    int sum = 0;
    for (int d = 0; d < radix; ++d) {
      int v = c[d];
      c[d] = sum;
      sum += v;
    }
  }

  Item* src = buf_a.data();
  Item* dst = buf_b.data();
  for (int p = 0; p < passes; ++p) {
    int* c = counts.data() + p * radix;
    int shift = p * bits;
    for (int i = 0; i < n; ++i) {
      unsigned int d = (unsigned int)(src[i].key >> shift) & mask;
      dst[c[d]++] = src[i];
    }
    std::swap(src, dst);
  }

  std::vector<int> sa(n);
  int dup_count = 0;
  for (int i = 1; i < n; ++i) dup_count += (src[i].key == src[i - 1].key);
  if (dup_count <= 256) {
    for (int i = 0; i < n; ++i) sa[i] = src[i].idx;
    if (dup_count > 0) {
      int i = 0;
      while (i < n) {
        int j = i + 1;
        while (j < n && src[j].key == src[i].key) ++j;
        if (j - i > 1) {
          std::sort(sa.data() + i, sa.data() + j, [&](int l, int r) {
            int limit = n - (l > r ? l : r);
            for (int k = 1; k < limit; ++k) {
              if (s[l + k] != s[r + k]) return s[l + k] < s[r + k];
            }
            return l > r;
          });
        }
        i = j;
      }
    }
    return sa;
  }

  std::vector<int> s2(n);
  int now = 0;
  s2[src[0].idx] = 0;
  for (int i = 1; i < n; ++i) {
    if (src[i].key != src[i - 1].key) ++now;
    s2[src[i].idx] = now;
  }
  std::vector<int> cnt_pool;
  std::vector<int> bkt;
  cnt_pool.reserve(now + 1 + n / 2);
  bkt.reserve(now + 1);
  sa_is_core(s2.data(), n, now, sa.data(), cnt_pool, 0, bkt);
  return sa;
}

template <typename T, typename Compare>
std::vector<int> suffix_array_compress_sort(const std::vector<T>& s,
                                            Compare compare) {
  int n = (int)s.size();
  std::vector<int> idx(n);
  std::iota(idx.begin(), idx.end(), 0);
  std::sort(idx.begin(), idx.end(),
            [&](int l, int r) { return compare(s[l], s[r]); });
  std::vector<int> s2(n);
  int now = 0;
  s2[idx[0]] = 0;
  for (int i = 1; i < n; ++i) {
    if (compare(s[idx[i - 1]], s[idx[i]])) ++now;
    s2[idx[i]] = now;
  }
  if (now == n - 1) return idx;
  std::vector<int> sa(n);
  std::vector<int> cnt_pool;
  std::vector<int> bkt;
  cnt_pool.reserve(now + 1 + n / 2);
  bkt.reserve(now + 1);
  sa_is_core(s2.data(), n, now, sa.data(), cnt_pool, 0, bkt);
  return sa;
}

}  // namespace internal_suffix_array

inline std::vector<int> suffix_array(const std::vector<int>& s, int upper) {
  assert(upper >= 0);
  for (int d : s) assert((unsigned int)d <= (unsigned int)upper);
  int n = (int)s.size();
  assert(n < (1 << 30));
  if (n == 0) return {};
  std::vector<int> sa(n);
  if (n <= internal_suffix_array::threshold_naive) {
    internal_suffix_array::sa_naive(s.data(), n, sa.data());
    return sa;
  }
  if (upper > 4096 && upper > (long long)n * 8) {
    return internal_suffix_array::suffix_array_radix(s, 0, (unsigned int)upper);
  }
  std::vector<int> cnt_pool;
  std::vector<int> bkt;
  cnt_pool.reserve(upper + 1 + n / 2);
  bkt.reserve(upper + 1);
  if (upper <= 255 && n >= 4096) {
    std::vector<unsigned char> s2(n);
    for (int i = 0; i < n; ++i) s2[i] = (unsigned char)s[i];
    internal_suffix_array::sa_is_core(s2.data(), n, upper, sa.data(), cnt_pool,
                                      0, bkt);
    return sa;
  }
  internal_suffix_array::sa_is_core(s.data(), n, upper, sa.data(), cnt_pool, 0,
                                    bkt);
  return sa;
}

inline std::vector<int> suffix_array(std::string_view s) {
  int n = (int)s.size();
  assert(n < (1 << 30));
  if (n == 0) return {};
  std::vector<int> sa(n);
  const unsigned char* p = (const unsigned char*)s.data();
  if (n <= internal_suffix_array::threshold_naive) {
    internal_suffix_array::sa_naive(p, n, sa.data());
    return sa;
  }
  std::vector<int> cnt_pool;
  std::vector<int> bkt;
  cnt_pool.reserve(256 + n);
  bkt.reserve(n);
  internal_suffix_array::sa_is_core(p, n, 255, sa.data(), cnt_pool, 0, bkt);
  return sa;
}

template <typename T, typename Compare = std::less<>>
  requires requires(Compare& cmp, const T& a, const T& b) {
    { cmp(a, b) } -> std::convertible_to<bool>;
  }
std::vector<int> suffix_array(const std::vector<T>& s,
                              Compare compare = Compare{}) {
  int n = (int)s.size();
  assert(n < (1 << 30));
  if (n == 0) return {};
  if (n == 1) return {0};
  constexpr bool is_default_less =
      std::is_same_v<std::decay_t<Compare>, std::less<>> ||
      std::is_same_v<std::decay_t<Compare>, std::less<T>>;
  if constexpr (std::is_same_v<T, bool>) {
    if (n <= internal_suffix_array::threshold_naive) {
      std::vector<int> sa(n);
      internal_suffix_array::sa_naive(s, n, sa.data(), compare);
      return sa;
    }
    if constexpr (is_default_less) {
      std::vector<unsigned char> s2(n);
      for (int i = 0; i < n; ++i) s2[i] = (unsigned char)s[i];
      std::vector<int> sa(n);
      std::vector<int> cnt_pool;
      std::vector<int> bkt;
      cnt_pool.reserve(2 + n);
      bkt.reserve(n);
      internal_suffix_array::sa_is_core(s2.data(), n, 1, sa.data(), cnt_pool, 0,
                                        bkt);
      return sa;
    } else {
      return internal_suffix_array::suffix_array_compress_sort(s, compare);
    }
  } else {
    if (n <= internal_suffix_array::threshold_naive) {
      std::vector<int> sa(n);
      internal_suffix_array::sa_naive(s.data(), n, sa.data(), compare);
      return sa;
    }
    if constexpr (is_default_less && internal::is_integer_v<T>) {
      if constexpr (sizeof(T) == 1) {
        std::vector<int> sa(n);
        std::vector<int> cnt_pool;
        std::vector<int> bkt;
        cnt_pool.reserve(256 + n);
        bkt.reserve(n);
        if constexpr (std::is_unsigned_v<T>) {
          internal_suffix_array::sa_is_core((const unsigned char*)s.data(), n,
                                            255, sa.data(), cnt_pool, 0, bkt);
        } else {
          std::vector<unsigned char> s2(n);
          for (int i = 0; i < n; ++i) s2[i] = (unsigned char)((int)s[i] + 128);
          internal_suffix_array::sa_is_core(s2.data(), n, 255, sa.data(),
                                            cnt_pool, 0, bkt);
        }
        return sa;
      } else {
        using U = std::conditional_t<
            sizeof(T) == 2, unsigned short,
            std::conditional_t<
                sizeof(T) == 4, unsigned int,
                std::conditional_t<sizeof(T) == 8, unsigned long long,
                                   unsigned __int128>>>;
        T min_v = s[0];
        T max_v = s[0];
        for (int i = 1; i < n; ++i) {
          if (s[i] < min_v) min_v = s[i];
          if (s[i] > max_v) max_v = s[i];
        }
        U range = (U)max_v - (U)min_v;
        if (range == 0) {
          std::vector<int> sa(n);
          for (int i = 0; i < n; ++i) sa[i] = n - 1 - i;
          return sa;
        }
        if (range <= (U)std::max((long long)n * 2, 4096LL)) {
          std::vector<int> sa(n);
          std::vector<int> cnt_pool;
          std::vector<int> bkt;
          int upper = (int)range;
          cnt_pool.reserve(upper + 1 + n / 2);
          bkt.reserve(upper + 1);
          if (range <= 255) {
            std::vector<unsigned char> s2(n);
            for (int i = 0; i < n; ++i) {
              s2[i] = (unsigned char)((U)s[i] - (U)min_v);
            }
            internal_suffix_array::sa_is_core(s2.data(), n, upper, sa.data(),
                                              cnt_pool, 0, bkt);
            return sa;
          }
          if constexpr (std::is_same_v<T, int>) {
            if (min_v == 0) {
              internal_suffix_array::sa_is_core(s.data(), n, upper, sa.data(),
                                                cnt_pool, 0, bkt);
              return sa;
            }
          }
          std::vector<int> s2(n);
          for (int i = 0; i < n; ++i) s2[i] = (int)((U)s[i] - (U)min_v);
          internal_suffix_array::sa_is_core(s2.data(), n, upper, sa.data(),
                                            cnt_pool, 0, bkt);
          return sa;
        }
        if (n >= 256) {
          return internal_suffix_array::suffix_array_radix(s, min_v, range);
        }
        return internal_suffix_array::suffix_array_compress_sort(s, compare);
      }
    } else {
      return internal_suffix_array::suffix_array_compress_sort(s, compare);
    }
  }
}

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_STRING_SUFFIX_ARRAY_HPP_
