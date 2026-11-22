// Repository: Library3372
// File: library3372/string/suffix_array.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/string/suffix_array.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_STRING_SUFFIX_ARRAY_HPP_
#define LIBRARY3372_LIBRARY3372_STRING_SUFFIX_ARRAY_HPP_

#include <algorithm>
#include <bit>
#include <cassert>
#include <cstring>
#include <numeric>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "library3372/internal/type_traits/is_integer.hpp"

namespace library3372 {
namespace internal_suffix_array {

inline constexpr int kThresholdNaive = 32;
inline constexpr int kTagBit = (int)0x80000000U;
inline constexpr int kLmsBit = 0x40000000;

template <typename T> inline void sa_naive(const T* s, int n, int* sa) {
  for (int i = 0; i < n; ++i) sa[i] = i;
  auto cmp = [s, n](int l, int r) {
    int limit = n - (l > r ? l : r);
    for (int k = 0; k < limit; ++k) {
      if (s[l + k] < s[r + k]) return true;
      if (s[r + k] < s[l + k]) return false;
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
    fn(i + 1, next_c, is_lms);
    prev_s = cur_s;
    next_c = c;
  }
}

template <typename CharT>
inline void induce_l(const CharT* s, int n, const int* cnt, int* bkt, int upper,
                     int* sa) {
  set_buckets(cnt, bkt, upper, false);
  int c_last = (int)s[n - 1];
  sa[bkt[c_last]++] = ((int)s[n - 2] < c_last) ? ((n - 1) | kTagBit) : (n - 1);
  int dummy = 0;
  for (int i = 0; i < n; ++i) {
    int v = sa[i];
    if (v > 0) {
      // Initial LMS entries carry kLmsBit so they can be cleared without a
      // separate L/S array; L-type predecessors are tagged if u-1 is S-type.
      int is_lms = v >> 30;
      int* clr = is_lms ? &sa[i] : &dummy;
      *clr = -1;
      v &= ~kLmsBit;
      int u = v - 1;
      int cu = (int)s[u];
      sa[bkt[cu]++] = (u > 0 && (int)s[u - 1] < cu) ? (u | kTagBit) : u;
    }
  }
}

template <bool MarkLms, typename CharT>
inline void induce_s(const CharT* s, int n, const int* cnt, int* bkt, int upper,
                     int* sa) {
  set_buckets(cnt, bkt, upper, true);
  for (int i = n - 1; i >= 0; --i) {
    int v = sa[i];
    if (v < -1) {
      v &= ~kTagBit;
      sa[i] = v;
      int u = v - 1;
      int cu = (int)s[u];
      if constexpr (MarkLms) {
        if (u > 0) {
          sa[--bkt[cu]] = ((int)s[u - 1] <= cu) ? (u | kTagBit) : (u | kLmsBit);
        } else {
          sa[--bkt[cu]] = 0;
        }
      } else {
        sa[--bkt[cu]] = (u > 0 && (int)s[u - 1] <= cu) ? (u | kTagBit) : u;
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
  if (n <= kThresholdNaive) {
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
  ++cnt[(int)s[n - 1]];
  for_each_lms_reverse(s, n, [&](int pos, int, int is_lms) {
    ++cnt[(int)s[pos - 1]];
    m += is_lms;
    last_lms = is_lms ? pos : last_lms;
  });

  std::fill(sa, sa + n, -1);

  int dummy = 0;
  if (m >= 1) {
    set_buckets(cnt, bkt, upper, true);
    if (m == 1) {
      sa[--bkt[(int)s[last_lms]]] = last_lms | kLmsBit;
    } else {
      for_each_lms_reverse(s, n, [&](int pos, int next_c, int is_lms) {
        int b = bkt[next_c] - 1;
        int* bp = is_lms ? &bkt[next_c] : &dummy;
        *bp = b;
        int* dst = is_lms ? &sa[b] : &dummy;
        *dst = pos | kLmsBit;
      });
    }
  }

  induce_l(s, n, cnt, bkt, upper, sa);
  if (m <= 1) {
    induce_s<false>(s, n, cnt, bkt, upper, sa);
    return;
  }
  induce_s<true>(s, n, cnt, bkt, upper, sa);

  int k = 0;
  for (int i = 0; i < n; ++i) {
    int v = sa[i];
    sa[k] = v & ~kLmsBit;
    k += (v >> 30) & 1;
  }

  // No two adjacent indices can be LMS, so (p >> 1) uniquely maps every LMS
  // position into sa[m .. n - 1] without an external lms_map array.
  std::fill(sa + m, sa + n, -1);
  {
    int prev_lms = n;
    for_each_lms_reverse(s, n, [&](int p, int, int is_lms) {
      int* dst = is_lms ? &sa[m + (p >> 1)] : &dummy;
      // Sentinel-terminated last LMS gets length 1 (non-last LMS have len >= 3).
      *dst = (prev_lms == n) ? 1 : (prev_lms - p + 1);
      prev_lms = is_lms ? p : prev_lms;
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
      bool diff = (len != prev_len);
      if (!diff) {
        if constexpr (sizeof(CharT) == 1) {
          if (len > 16) {
            diff = (std::memcmp(s + prev_p, s + p, (unsigned long)len) != 0);
          } else {
            for (int d = 0; d < len; ++d) {
              if (s[prev_p + d] != s[p + d]) {
                diff = true;
                break;
              }
            }
          }
        } else {
          for (int d = 0; d < len; ++d) {
            if (s[prev_p + d] != s[p + d]) {
              diff = true;
              break;
            }
          }
        }
      }
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
    int j = 0;
    for (int i = m; i < n; ++i) {
      if (sa[i] >= 0) rec_s[j++] = sa[i];
    }
    sa_is_core<int>(rec_s, m, rec_upper, sa, cnt_pool, cnt_offset + upper + 1,
                    bkt_vec);
    j = m;
    for_each_lms_reverse(s, n, [&](int p, int, int is_lms) {
      j -= is_lms;
      int* dst = is_lms ? &sa[m + j] : &dummy;
      *dst = p;
    });
    for (int i = 0; i < m; ++i) sa[i] = sa[m + sa[i]];
    cnt = cnt_pool.data() + cnt_offset;
    bkt = bkt_vec.data();
  }

  std::fill(sa + m, sa + n, -1);
  set_buckets(cnt, bkt, upper, true);
  for (int i = m - 1; i >= 0; --i) {
    int p = sa[i];
    sa[i] = -1;
    sa[--bkt[(int)s[p]]] = p | kLmsBit;
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
  constexpr int kBits = 10;
  constexpr int kRadix = 1 << kBits;
  constexpr unsigned int kMask = kRadix - 1;
  int total_bits = bit_width_u(range);
  int passes = (total_bits + kBits - 1) / kBits;

  struct Item {
    U key;
    int idx;
  };
  std::vector<Item> buf_a(n);
  std::vector<Item> buf_b(n);
  std::vector<int> counts(passes * kRadix, 0);

  for (int i = 0; i < n; ++i) {
    U k = (U)s[i] - (U)min_v;
    buf_a[i] = {k, i};
    for (int p = 0; p < passes; ++p) {
      ++counts[p * kRadix + (int)((unsigned int)(k >> (p * kBits)) & kMask)];
    }
  }
  for (int p = 0; p < passes; ++p) {
    int* c = counts.data() + p * kRadix;
    int sum = 0;
    for (int d = 0; d < kRadix; ++d) {
      int v = c[d];
      c[d] = sum;
      sum += v;
    }
  }

  Item* src = buf_a.data();
  Item* dst = buf_b.data();
  for (int p = 0; p < passes; ++p) {
    int* c = counts.data() + p * kRadix;
    int shift = p * kBits;
    for (int i = 0; i < n; ++i) {
      unsigned int d = (unsigned int)(src[i].key >> shift) & kMask;
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

template <typename T>
std::vector<int> suffix_array_compress_sort(const std::vector<T>& s) {
  int n = (int)s.size();
  std::vector<int> idx(n);
  std::iota(idx.begin(), idx.end(), 0);
  std::sort(idx.begin(), idx.end(), [&](int l, int r) { return s[l] < s[r]; });
  std::vector<int> s2(n);
  int now = 0;
  s2[idx[0]] = 0;
  for (int i = 1; i < n; ++i) {
    if (s[idx[i - 1]] < s[idx[i]]) ++now;
    s2[idx[i]] = now;
  }
  std::vector<int> sa(n);
  if (now == n - 1) return idx;
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
  if (n <= internal_suffix_array::kThresholdNaive) {
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
  if (n <= internal_suffix_array::kThresholdNaive) {
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

template <typename T> std::vector<int> suffix_array(const std::vector<T>& s) {
  int n = (int)s.size();
  assert(n < (1 << 30));
  if (n == 0) return {};
  if (n == 1) return {0};
  if constexpr (std::is_same_v<T, bool>) {
    std::vector<unsigned char> s2(n);
    for (int i = 0; i < n; ++i) s2[i] = (unsigned char)s[i];
    std::vector<int> sa(n);
    if (n <= internal_suffix_array::kThresholdNaive) {
      internal_suffix_array::sa_naive(s2.data(), n, sa.data());
      return sa;
    }
    std::vector<int> cnt_pool;
    std::vector<int> bkt;
    cnt_pool.reserve(2 + n);
    bkt.reserve(n);
    internal_suffix_array::sa_is_core(s2.data(), n, 1, sa.data(), cnt_pool, 0,
                                      bkt);
    return sa;
  } else {
    if (n <= internal_suffix_array::kThresholdNaive) {
      std::vector<int> sa(n);
      internal_suffix_array::sa_naive(s.data(), n, sa.data());
      return sa;
    }
    if constexpr (internal::is_integer_v<T>) {
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
        return internal_suffix_array::suffix_array_compress_sort(s);
      }
    } else {
      return internal_suffix_array::suffix_array_compress_sort(s);
    }
  }
}

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_STRING_SUFFIX_ARRAY_HPP_
