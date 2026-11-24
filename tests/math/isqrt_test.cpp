// Repository: Library3372
// File: tests/math/isqrt_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/math/isqrt_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include "library3372/math/isqrt.hpp"

#include <cassert>
#include <climits>
#include <limits>
#include <random>

namespace {

using library3372::isqrt;

template <typename T> constexpr void check_isqrt(T n) {
  T s = isqrt(n);
  unsigned __int128 un = (unsigned __int128)n;
  unsigned __int128 us = (unsigned __int128)s;
  assert(us * us <= un);
  if (us < std::numeric_limits<unsigned long long>::max()) {
    assert((us + 1) * (us + 1) > un);
  }
}

constexpr bool test_constexpr() {
  if (isqrt(false) != false || isqrt(true) != true) return false;
  if (isqrt((char)0) != 0 || isqrt((char)15) != 3 || isqrt((char)127) != 11) {
    return false;
  }
  if (isqrt((signed char)0) != 0 || isqrt((signed char)100) != 10) return false;
  if (isqrt((unsigned char)255) != 15) return false;
  if (isqrt((short)32767) != 181) return false;
  if (isqrt((unsigned short)65535) != 255) return false;
  if (isqrt(0) != 0 || isqrt(1) != 1 || isqrt(8) != 2 || isqrt(9) != 3) {
    return false;
  }
  if (isqrt(INT_MAX) != 46340) return false;
  if (isqrt(UINT_MAX) != 65535U) return false;
  if (isqrt(LLONG_MAX) != 3037000499LL) return false;
  if (isqrt(ULLONG_MAX) != 4294967295ULL) return false;

  constexpr unsigned __int128 u128_max = ~(unsigned __int128)0;
  constexpr __int128 i128_max = (__int128)(u128_max >> 1);
  constexpr __int128 s_i128 = isqrt(i128_max);
  if ((unsigned __int128)s_i128 * (unsigned __int128)s_i128 >
      (unsigned __int128)i128_max) {
    return false;
  }
  if (((unsigned __int128)s_i128 + 1) * ((unsigned __int128)s_i128 + 1) <=
      (unsigned __int128)i128_max) {
    return false;
  }
  if (isqrt(u128_max) != (unsigned __int128)ULLONG_MAX) return false;
  if (isqrt((unsigned __int128)1 << 64) != ((unsigned __int128)1 << 32)) {
    return false;
  }
  if (isqrt(((unsigned __int128)1 << 64) - 1) != ULLONG_MAX >> 32) {
    return false;
  }
  return true;
}

template <typename T> void test_small_type() {
  using Lim = std::numeric_limits<T>;
  for (int v = 0; v <= (int)Lim::max(); ++v) {
    check_isqrt((T)v);
  }
}

void test_all_types_and_boundaries() {
  static_assert(test_constexpr());

  assert(isqrt(false) == false);
  assert(isqrt(true) == true);

  test_small_type<char>();
  test_small_type<signed char>();
  test_small_type<unsigned char>();
  test_small_type<short>();
  test_small_type<unsigned short>();

  for (int v = 0; v <= 10000; ++v) check_isqrt(v);
  for (unsigned int v = 0; v <= 10000; ++v) check_isqrt(v);

  // Around 16-bit, 32-bit, 53-bit, 64-bit, 128-bit boundaries
  for (long long k : {46340LL, 46341LL, 65535LL, 65536LL, 3037000499LL}) {
    unsigned long long base = (unsigned long long)k * (unsigned long long)k;
    for (long long d = -3; d <= 3; ++d) {
      unsigned long long u = base + (unsigned long long)d;
      if (d < 0 && base < (unsigned long long)(-d)) continue;
      check_isqrt(u);
      if (u <= (unsigned long long)LLONG_MAX) check_isqrt((long long)u);
      if (u <= (unsigned long long)INT_MAX) check_isqrt((int)u);
      if (u <= (unsigned long long)UINT_MAX) check_isqrt((unsigned int)u);
    }
  }

  for (int e = 0; e <= 63; ++e) {
    unsigned long long p = 1ULL << e;
    for (long long d = -3; d <= 3; ++d) {
      if (d < 0 && p < (unsigned long long)(-d)) continue;
      unsigned long long u = p + (unsigned long long)d;
      check_isqrt(u);
      if (u <= (unsigned long long)LLONG_MAX) check_isqrt((long long)u);
      check_isqrt((unsigned __int128)u);
      check_isqrt((__int128)u);
    }
  }

  check_isqrt((long)0);
  check_isqrt(LONG_MAX);
  check_isqrt(ULONG_MAX);
  check_isqrt(LLONG_MAX);
  check_isqrt(ULLONG_MAX);
  for (unsigned long long u = ULLONG_MAX - 20; u < ULLONG_MAX; ++u) {
    check_isqrt(u);
  }
  for (long long v = LLONG_MAX - 20; v < LLONG_MAX; ++v) {
    check_isqrt(v);
  }

  // 128-bit boundaries
  const unsigned __int128 u128_max = ~(unsigned __int128)0;
  const __int128 i128_max = (__int128)(u128_max >> 1);
  check_isqrt(u128_max);
  check_isqrt(i128_max);
  for (unsigned __int128 d = 0; d <= 20; ++d) {
    check_isqrt(u128_max - d);
    check_isqrt(i128_max - (__int128)d);
  }
  for (int e = 64; e <= 127; ++e) {
    unsigned __int128 p = (unsigned __int128)1 << e;
    for (int d = -3; d <= 3; ++d) {
      unsigned __int128 u = p + (unsigned __int128)(__int128)d;
      check_isqrt(u);
      if (u <= (unsigned __int128)i128_max) check_isqrt((__int128)u);
    }
  }
  for (unsigned __int128 k :
       {(unsigned __int128)1 << 32, ((unsigned __int128)1 << 63) - 1,
        (unsigned __int128)1 << 63, ((unsigned __int128)1 << 64) - 1}) {
    unsigned __int128 base = k * k;
    for (int d = -3; d <= 3; ++d) {
      if (d > 0 && u128_max - base < (unsigned __int128)d) continue;
      unsigned __int128 u = base + (unsigned __int128)(__int128)d;
      check_isqrt(u);
      if (u <= (unsigned __int128)i128_max) check_isqrt((__int128)u);
    }
  }
}

void test_random(std::mt19937_64& rng) {
  std::uniform_int_distribution<unsigned int> d32(0U, UINT_MAX);
  for (int i = 0; i < 10000; ++i) {
    unsigned int u = d32(rng);
    check_isqrt(u);
    check_isqrt((int)(u & (unsigned int)INT_MAX));
  }

  std::uniform_int_distribution<unsigned long long> d64(0ULL, ULLONG_MAX);
  for (int i = 0; i < 10000; ++i) {
    unsigned long long u = d64(rng);
    check_isqrt(u);
    check_isqrt((long long)(u & (unsigned long long)LLONG_MAX));
  }

  for (int i = 0; i < 5000; ++i) {
    unsigned __int128 u = ((unsigned __int128)d64(rng) << 64) | d64(rng);
    check_isqrt(u);
    check_isqrt((__int128)(u >> 1));
  }
}

}  // namespace

int main() {
  test_all_types_and_boundaries();

  std::mt19937_64 rng(0x3c6ef372fe94f82aULL);
  test_random(rng);
  return 0;
}
