// Repository: Library3372
// File: tests/math/kth_root_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/math/kth_root_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include "library3372/math/kth_root.hpp"

#include <cassert>
#include <climits>
#include <limits>
#include <random>

namespace {

using library3372::kth_root;

constexpr bool pow_le_u128(unsigned __int128 x, unsigned __int128 k,
                           unsigned __int128 u) {
  if (x <= 1) return x <= u;
  unsigned __int128 res = 1;
  while (k > 0) {
    if (k & 1) {
      if (__builtin_mul_overflow(res, x, &res) || res > u) return false;
    }
    k >>= 1;
    if (k == 0) return true;
    if (__builtin_mul_overflow(x, x, &x) || x > u) return false;
  }
  return true;
}

template <typename T, typename K> constexpr void check_kth_root(T n, K k) {
  T r = kth_root(n, k);
  unsigned __int128 un = (unsigned __int128)n;
  unsigned __int128 uk = (unsigned __int128)k;
  unsigned __int128 ur = (unsigned __int128)r;
  assert(pow_le_u128(ur, uk, un));
  if (ur < ~(unsigned __int128)0) {
    assert(!pow_le_u128(ur + 1, uk, un));
  }
}

constexpr bool test_constexpr() {
  if (kth_root(false, true) != false || kth_root(true, true) != true) {
    return false;
  }
  if (kth_root(false, 3) != false || kth_root(true, 3) != true) return false;
  if (kth_root(42, true) != 42) return false;
  if (kth_root((char)0, (char)3) != 0 || kth_root((char)27, (char)3) != 3 ||
      kth_root((char)127, (char)3) != 5) {
    return false;
  }
  if (kth_root((signed char)100, (signed char)2) != 10) return false;
  if (kth_root((unsigned char)255, (unsigned char)4) != 3) return false;
  if (kth_root((short)32767, (short)3) != 31) return false;
  if (kth_root((unsigned short)65535, (unsigned short)4) != 15) return false;

  if (kth_root(0, 1) != 0 || kth_root(1, 1) != 1 ||
      kth_root(12345, 1) != 12345) {
    return false;
  }
  if (kth_root(0, 3) != 0 || kth_root(1, 3) != 1 || kth_root(26, 3) != 2 ||
      kth_root(27, 3) != 3) {
    return false;
  }
  if (kth_root(INT_MAX, 3) != 1290) return false;
  if (kth_root(UINT_MAX, 3) != 1625U) return false;
  if (kth_root(LLONG_MAX, 3) != 2097151LL) return false;
  if (kth_root(ULLONG_MAX, 3) != 2642245ULL) return false;
  if (kth_root(ULLONG_MAX, 64) != 1ULL) return false;
  if (kth_root(ULLONG_MAX, 63) != 2ULL) return false;
  if (kth_root(ULLONG_MAX, ULLONG_MAX) != 1ULL) return false;

  constexpr unsigned __int128 u128_max = ~(unsigned __int128)0;
  constexpr __int128 i128_max = (__int128)(u128_max >> 1);
  constexpr __int128 r_i128 = kth_root(i128_max, 3);
  if (!pow_le_u128((unsigned __int128)r_i128, 3, (unsigned __int128)i128_max) ||
      pow_le_u128((unsigned __int128)r_i128 + 1, 3,
                  (unsigned __int128)i128_max)) {
    return false;
  }
  constexpr unsigned __int128 r_u128 = kth_root(u128_max, 5);
  if (!pow_le_u128(r_u128, 5, u128_max) ||
      pow_le_u128(r_u128 + 1, 5, u128_max)) {
    return false;
  }
  if (kth_root(u128_max, 127) != 2 || kth_root(u128_max, 128) != 1) {
    return false;
  }
  if (kth_root(u128_max, u128_max) != 1) return false;
  if (kth_root(u128_max, 1) != u128_max) return false;
  if (kth_root((unsigned __int128)1 << 90, 3) != ((unsigned __int128)1 << 30)) {
    return false;
  }
  if (kth_root(((unsigned __int128)1 << 90) - 1, 3) !=
      ((unsigned __int128)1 << 30) - 1) {
    return false;
  }
  return true;
}

template <typename T> void test_small_type(int max_n, int max_k) {
  for (int v = 0; v <= max_n; ++v) {
    for (int k = 1; k <= max_k; ++k) check_kth_root((T)v, k);
  }
}

void test_all_types_and_boundaries() {
  static_assert(test_constexpr());

  assert(kth_root(false, true) == false);
  assert(kth_root(true, true) == true);
  assert(kth_root(false, 5) == false);
  assert(kth_root(true, 5) == true);
  assert(kth_root(123456789LL, true) == 123456789LL);

  test_small_type<char>(127, 10);
  test_small_type<signed char>(127, 10);
  test_small_type<unsigned char>(255, 10);
  test_small_type<short>(500, 18);
  test_small_type<unsigned short>(500, 18);

  for (int v = 0; v <= 200; ++v) {
    for (int k = 1; k <= 12; ++k) {
      check_kth_root(v, k);
      check_kth_root((unsigned int)v, (unsigned int)k);
    }
  }
  for (int k = 1; k <= 16; ++k) {
    check_kth_root((short)32767, (signed char)k);
    check_kth_root((unsigned short)65535, (unsigned char)k);
  }

  // Powers of 2 boundaries up to 128-bit
  const unsigned __int128 u128_max = ~(unsigned __int128)0;
  const __int128 i128_max = (__int128)(u128_max >> 1);
  for (int e = 0; e <= 127; ++e) {
    unsigned __int128 p = (unsigned __int128)1 << e;
    for (int d = -2; d <= 2; ++d) {
      if (d < 0 && p < (unsigned __int128)(-d)) continue;
      if (d > 0 && u128_max - p < (unsigned __int128)d) continue;
      unsigned __int128 u = p + (unsigned __int128)(__int128)d;
      for (int k : {1, 2, 3, 4, 5, 7, 10, 15, 20, 31, 32, 33, 63, 64, 65, 127,
                    128, 129}) {
        check_kth_root(u, k);
        if (u <= (unsigned __int128)i128_max) check_kth_root((__int128)u, k);
        if (u <= ULLONG_MAX) check_kth_root((unsigned long long)u, k);
        if (u <= (unsigned __int128)LLONG_MAX) check_kth_root((long long)u, k);
        if (u <= UINT_MAX) check_kth_root((unsigned int)u, k);
        if (u <= (unsigned __int128)INT_MAX) check_kth_root((int)u, k);
      }
    }
  }

  // Exact k-th powers m^k +/- 1 for 64-bit and 128-bit
  for (unsigned int k = 1; k <= 64; ++k) {
    for (unsigned long long m :
         {1ULL, 2ULL, 3ULL, 5ULL, 10ULL, 100ULL, 1000ULL, 65535ULL, 65536ULL,
          2097151ULL, 2642245ULL, 1000000000ULL, 3037000499ULL}) {
      unsigned __int128 p = 1;
      bool ov = false;
      for (unsigned int j = 0; j < k; ++j) {
        if (__builtin_mul_overflow(p, (unsigned __int128)m, &p)) {
          ov = true;
          break;
        }
      }
      if (ov) continue;
      for (int d = -2; d <= 2; ++d) {
        if (d < 0 && p < (unsigned __int128)(-d)) continue;
        if (d > 0 && u128_max - p < (unsigned __int128)d) continue;
        unsigned __int128 u = p + (unsigned __int128)(__int128)d;
        check_kth_root(u, k);
        if (u <= (unsigned __int128)i128_max) check_kth_root((__int128)u, k);
        if (u <= ULLONG_MAX) check_kth_root((unsigned long long)u, k);
        if (u <= (unsigned __int128)LLONG_MAX) check_kth_root((long long)u, k);
      }
    }
  }

  // Max values and cross-type K
  for (int k = 1; k <= 130; ++k) {
    check_kth_root(LONG_MAX, (long)k);
    check_kth_root(ULONG_MAX, (unsigned long)k);
    check_kth_root(LLONG_MAX, (long long)k);
    check_kth_root(ULLONG_MAX, (unsigned long long)k);
    check_kth_root(i128_max, (__int128)k);
    check_kth_root(u128_max, (unsigned __int128)k);
  }
  check_kth_root(ULLONG_MAX, LLONG_MAX);
  check_kth_root(ULLONG_MAX, ULLONG_MAX);
  check_kth_root(u128_max, i128_max);
  check_kth_root(u128_max, u128_max);
  check_kth_root(0ULL, ULLONG_MAX);
  check_kth_root((unsigned __int128)0, u128_max);
}

void test_random(std::mt19937_64& rng) {
  std::uniform_int_distribution<unsigned int> d32(0U, UINT_MAX);
  std::uniform_int_distribution<int> dk32(1, 35);
  for (int i = 0; i < 5000; ++i) {
    unsigned int u = d32(rng);
    int k = dk32(rng);
    check_kth_root(u, k);
    check_kth_root((int)(u & (unsigned int)INT_MAX), k);
  }

  std::uniform_int_distribution<unsigned long long> d64(0ULL, ULLONG_MAX);
  std::uniform_int_distribution<int> dk64(1, 66);
  for (int i = 0; i < 5000; ++i) {
    unsigned long long u = d64(rng);
    int k = dk64(rng);
    check_kth_root(u, k);
    check_kth_root((long long)(u & (unsigned long long)LLONG_MAX), k);
  }

  // Small k (3..10) on 64-bit and 128-bit
  std::uniform_int_distribution<int> dk_small(3, 10);
  for (int i = 0; i < 5000; ++i) {
    unsigned long long u = d64(rng);
    int k = dk_small(rng);
    check_kth_root(u, k);
    check_kth_root((long long)(u & (unsigned long long)LLONG_MAX), k);
  }

  std::uniform_int_distribution<int> dk128(1, 130);
  for (int i = 0; i < 3000; ++i) {
    unsigned __int128 u = ((unsigned __int128)d64(rng) << 64) | d64(rng);
    int k = dk128(rng);
    check_kth_root(u, k);
    check_kth_root((__int128)(u >> 1), k);
    check_kth_root(u, dk_small(rng));
    check_kth_root(u, d64(rng) | 1ULL);
  }
}

}  // namespace

int main() {
  test_all_types_and_boundaries();

  std::mt19937_64 rng(0x9e3779b97f4a7c15ULL);
  test_random(rng);
  return 0;
}
