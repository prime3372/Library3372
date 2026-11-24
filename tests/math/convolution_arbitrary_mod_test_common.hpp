// Repository: Library3372
// File: tests/math/convolution_arbitrary_mod_test_common.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/math/convolution_arbitrary_mod_test_common.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_TESTS_MATH_CONVOLUTION_ARBITRARY_MOD_TEST_COMMON_HPP_
#define LIBRARY3372_TESTS_MATH_CONVOLUTION_ARBITRARY_MOD_TEST_COMMON_HPP_

#include <algorithm>
#include <cassert>
#include <climits>
#include <random>
#include <type_traits>
#include <utility>
#include <vector>

#include "library3372/math/convolution_arbitrary_mod.hpp"
#include "library3372/math/dynamic_modint.hpp"
#include "library3372/math/static_modint.hpp"

namespace {

using library3372::convolution_arbitrary_mod;
using library3372::DynamicModInt;
using library3372::StaticModInt;

template <typename T> unsigned long long ref_normalize(T v, long long mod) {
  if constexpr (library3372::internal::is_signed_integer_v<T>) {
    long long r = (long long)(v % (__int128)mod);
    if (r < 0) r += mod;
    return (unsigned long long)r;
  } else if constexpr (library3372::internal::is_unsigned_integer_v<T>) {
    return (unsigned long long)(v % (unsigned __int128)mod);
  } else {
    return (unsigned long long)v.val();
  }
}

template <typename T>
std::vector<T> naive_convolution_arbitrary_mod(const std::vector<T>& a,
                                               const std::vector<T>& b,
                                               long long mod) {
  if (a.empty() || b.empty()) return {};
  int n = (int)a.size();
  int k = (int)b.size();
  std::vector<T> c(n + k - 1);
  unsigned long long umod = (unsigned long long)mod;
  for (int i = 0; i < n; ++i) {
    unsigned long long av = ref_normalize(a[i], mod);
    for (int j = 0; j < k; ++j) {
      unsigned long long bv = ref_normalize(b[j], mod);
      unsigned long long prod =
          (unsigned long long)((unsigned __int128)av * bv % umod);
      if constexpr (library3372::internal::is_integer_v<T>) {
        unsigned long long cur = (unsigned long long)c[i + j];
        unsigned __int128 sum = (unsigned __int128)cur + prod;
        c[i + j] = (T)(unsigned long long)(sum % umod);
      } else {
        c[i + j] += T::raw((unsigned int)prod);
      }
    }
  }
  return c;
}

template <int m> void test_static_pair(std::mt19937_64& rng, int n, int k) {
  std::uniform_int_distribution<int> dist(0, m - 1);
  std::vector<StaticModInt<m>> a(n), b(k);
  for (int i = 0; i < n; ++i) a[i] = StaticModInt<m>::raw(dist(rng));
  for (int i = 0; i < k; ++i) b[i] = StaticModInt<m>::raw(dist(rng));
  assert(convolution_arbitrary_mod(a, b) ==
         naive_convolution_arbitrary_mod(a, b, m));
  if (n <= 1024) {
    assert(convolution_arbitrary_mod(a, a) ==
           naive_convolution_arbitrary_mod(a, a, m));
  }
}

template <int id>
void test_dynamic_pair(std::mt19937_64& rng, int mod, int n, int k) {
  DynamicModInt<id>::set_mod(mod);
  std::uniform_int_distribution<int> dist(0, mod - 1);
  std::vector<DynamicModInt<id>> a(n), b(k);
  for (int i = 0; i < n; ++i) a[i] = DynamicModInt<id>::raw(dist(rng));
  for (int i = 0; i < k; ++i) b[i] = DynamicModInt<id>::raw(dist(rng));
  assert(convolution_arbitrary_mod(a, b) ==
         naive_convolution_arbitrary_mod(a, b, mod));
  if (n <= 1024) {
    assert(convolution_arbitrary_mod(a, a) ==
           naive_convolution_arbitrary_mod(a, a, mod));
  }
}

template <int m, typename T>
void test_integer_pair(std::mt19937_64& rng, int n, int k) {
  std::vector<T> a(n), b(k);
  for (int i = 0; i < n; ++i) {
    if constexpr (std::is_same_v<T, bool>) {
      a[i] = (rng() & 1ULL) != 0;
    } else if constexpr (library3372::internal::is_signed_integer_v<T>) {
      long long r = (long long)(rng() % (unsigned long long)(3LL * m));
      r -= (long long)m;
      a[i] = (T)r;
    } else {
      unsigned long long r = rng() % (unsigned long long)(2ULL * m + 5ULL);
      a[i] = (T)r;
    }
  }
  for (int i = 0; i < k; ++i) {
    if constexpr (std::is_same_v<T, bool>) {
      b[i] = (rng() & 1ULL) != 0;
    } else if constexpr (library3372::internal::is_signed_integer_v<T>) {
      long long r = (long long)(rng() % (unsigned long long)(3LL * m));
      r -= (long long)m;
      b[i] = (T)r;
    } else {
      unsigned long long r = rng() % (unsigned long long)(2ULL * m + 5ULL);
      b[i] = (T)r;
    }
  }
  std::vector<T> expected = naive_convolution_arbitrary_mod(a, b, m);
  assert(convolution_arbitrary_mod(a, b, m) == expected);
  assert(convolution_arbitrary_mod<m>(a, b) == expected);
  if (n <= 128) {
    std::vector<T> expected_self = naive_convolution_arbitrary_mod(a, a, m);
    assert(convolution_arbitrary_mod(a, a, m) == expected_self);
    assert(convolution_arbitrary_mod<m>(a, a) == expected_self);
  }
}

inline void test_empty_and_small() {
  using SMint = StaticModInt<1000000007>;
  assert(convolution_arbitrary_mod(std::vector<SMint>{}, std::vector<SMint>{})
             .empty());
  assert(convolution_arbitrary_mod(std::vector<SMint>{1}, std::vector<SMint>{})
             .empty());
  assert(convolution_arbitrary_mod(std::vector<SMint>{}, std::vector<SMint>{2})
             .empty());

  using DMint = DynamicModInt<0>;
  DMint::set_mod(1000000000);
  assert(convolution_arbitrary_mod(std::vector<DMint>{}, std::vector<DMint>{})
             .empty());
  assert(
      convolution_arbitrary_mod(std::vector<DMint>{1}, std::vector<DMint>{2}) ==
      std::vector<DMint>{2});

  assert(convolution_arbitrary_mod(std::vector<int>{}, std::vector<int>{}, 12)
             .empty());
  assert(convolution_arbitrary_mod<12>(std::vector<int>{}, std::vector<int>{})
             .empty());

  // mod = 1
  using Mint1 = StaticModInt<1>;
  std::vector<Mint1> m1_a(100, 0), m1_b(120, 0);
  assert(convolution_arbitrary_mod(m1_a, m1_b) == std::vector<Mint1>(219, 0));
  std::vector<int> i1_a(100, 123), i1_b(120, -456);
  assert(convolution_arbitrary_mod(i1_a, i1_b, 1) == std::vector<int>(219, 0));

  // Large extreme integer values (LLONG_MIN, ULLONG_MAX, __int128)
  const __int128 i128_min = (__int128)((unsigned __int128)1 << 127);
  const unsigned __int128 u128_max = ~(unsigned __int128)0;
  std::vector<long long> lla = {LLONG_MIN, LLONG_MAX, -1LL, 0LL, 1000000006LL};
  std::vector<long long> llb = {-1LL, LLONG_MAX, LLONG_MIN, 42LL};
  assert(convolution_arbitrary_mod(lla, llb, 1000000007) ==
         naive_convolution_arbitrary_mod(lla, llb, 1000000007));

  std::vector<__int128> i128a(70, i128_min);
  std::vector<__int128> i128b(70, ~i128_min);
  assert(convolution_arbitrary_mod(i128a, i128b, 1000000007) ==
         naive_convolution_arbitrary_mod(i128a, i128b, 1000000007));

  std::vector<unsigned __int128> u128a(70, u128_max);
  std::vector<unsigned __int128> u128b(70, u128_max - 1);
  assert(convolution_arbitrary_mod(u128a, u128b, 1000000009) ==
         naive_convolution_arbitrary_mod(u128a, u128b, 1000000009));
}

inline void test_worst_case_max_value() {
  // Check worst-case max value under mod = INT_MAX around NTT threshold.
  using Mint = StaticModInt<INT_MAX>;
  std::vector<Mint> a(200, Mint::raw(INT_MAX - 1));
  std::vector<Mint> b(200, Mint::raw(INT_MAX - 1));
  assert(convolution_arbitrary_mod(a, b) ==
         naive_convolution_arbitrary_mod(a, b, INT_MAX));
}

inline void test_length_over_2_24() {
  {
    using Mint = StaticModInt<1000000007>;
    constexpr int n = (1 << 25) - 1;
    std::vector<Mint> a(n, Mint::raw(1));
    std::vector<Mint> b = {Mint::raw(2), Mint::raw(3)};
    std::vector<Mint> c = convolution_arbitrary_mod(a, b);
    assert((int)c.size() == (1 << 25));
    assert(c[0].val() == 2);
    assert(c[1].val() == 5);
    assert(c[n - 1].val() == 5);
    assert(c[n].val() == 3);
  }
  {
    constexpr int n = (1 << 24) + 1;
    std::vector<long long> a(n, LLONG_MAX - 1);
    std::vector<long long> b = {2};
    std::vector<long long> c = convolution_arbitrary_mod(a, b, LLONG_MAX);
    assert((int)c.size() == n);
    assert(c[0] == LLONG_MAX - 2);
    assert(c[n - 1] == LLONG_MAX - 2);
  }
}

inline void run_convolution_arbitrary_mod_tests() {
  test_empty_and_small();
  test_worst_case_max_value();
  test_length_over_2_24();

  std::mt19937_64 rng(0x13572468abcdef01ULL);

  const int sizes[] = {1,  2,  3,  7,  8,   15,  16,  31,  32,  59,  60,
                       61, 63, 64, 65, 127, 128, 129, 256, 257, 513, 1024};
  for (int n : sizes) {
    for (int k : sizes) {
      if ((n > 200 && k > 200) && (n != k)) continue;
      test_static_pair<1000000007>(rng, n, k);
      test_static_pair<1000000000>(rng, n, k);
      test_static_pair<INT_MAX>(rng, n, k);
    }
  }

  for (int mod : {2, 12, 1000000007, 1000000000, INT_MAX}) {
    for (int n : {1, 8, 60, 61, 130, 300}) {
      for (int k : {1, 8, 60, 61, 130, 300}) {
        test_dynamic_pair<1>(rng, mod, n, k);
      }
    }
  }

  for (int n : {1, 16, 60, 61, 130}) {
    for (int k : {1, 16, 60, 61, 130}) {
      test_integer_pair<2, bool>(rng, n, k);
      test_integer_pair<12, signed char>(rng, n, k);
      test_integer_pair<12, unsigned char>(rng, n, k);
      test_integer_pair<10000, short>(rng, n, k);
      test_integer_pair<10000, unsigned short>(rng, n, k);
      test_integer_pair<1000000007, int>(rng, n, k);
      test_integer_pair<1000000007, unsigned int>(rng, n, k);
      test_integer_pair<1000000007, long long>(rng, n, k);
      test_integer_pair<1000000007, unsigned long long>(rng, n, k);
      test_integer_pair<INT_MAX, __int128>(rng, n, k);
      test_integer_pair<INT_MAX, unsigned __int128>(rng, n, k);
    }
  }

  // Larger randomized size sanity checks (> 4096)
  test_static_pair<1000000007>(rng, 4096, 4096);
  test_dynamic_pair<2>(rng, 1000000000, 4096, 4097);

  // 64-bit arbitrary mod (> INT_MAX, up to LLONG_MAX)
  constexpr long long mods64[] = {2147483648LL, (1LL << 40) + 7LL,
                                  1000000000000000000LL, LLONG_MAX};
  for (long long mod : mods64) {
    for (auto [n, k] : std::vector<std::pair<int, int>>{
             {1, 1}, {1, 70}, {60, 60}, {61, 61}, {129, 130}, {256, 257}}) {
      std::vector<long long> a(n), b(k);
      for (int i = 0; i < n; ++i) a[i] = (long long)rng();
      for (int i = 0; i < k; ++i) b[i] = (long long)rng();
      assert(convolution_arbitrary_mod(a, b, mod) ==
             naive_convolution_arbitrary_mod(a, b, mod));
      assert(convolution_arbitrary_mod(a, a, mod) ==
             naive_convolution_arbitrary_mod(a, a, mod));
    }
  }
  {
    std::vector<long long> a(200, LLONG_MAX - 1);
    std::vector<long long> b(200, LLONG_MAX - 1);
    assert(convolution_arbitrary_mod<LLONG_MAX>(a, b) ==
           naive_convolution_arbitrary_mod(a, b, LLONG_MAX));
    assert(convolution_arbitrary_mod<1000000000000000007LL>(a, b) ==
           naive_convolution_arbitrary_mod(a, b, 1000000000000000007LL));
  }
}

}  // namespace

#endif  // LIBRARY3372_TESTS_MATH_CONVOLUTION_ARBITRARY_MOD_TEST_COMMON_HPP_
