// Repository: Library3372
// File: tests/math/convolution_test_common.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/math/convolution_test_common.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_TESTS_MATH_CONVOLUTION_TEST_COMMON_HPP_
#define LIBRARY3372_TESTS_MATH_CONVOLUTION_TEST_COMMON_HPP_

#include <algorithm>
#include <cassert>
#include <climits>
#include <random>
#include <type_traits>
#include <utility>
#include <vector>

#include "library3372/math/convolution.hpp"
#include "library3372/math/static_modint.hpp"

namespace {

using library3372::convolution;
using library3372::StaticModInt;

template <int m, typename T>
std::vector<T> naive_convolution_mod(const std::vector<T>& a,
                                     const std::vector<T>& b) {
  if (a.empty() || b.empty()) return {};
  int n = (int)a.size();
  int k = (int)b.size();
  std::vector<T> c(n + k - 1);
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < k; ++j) {
      if constexpr (std::is_same_v<T, StaticModInt<m>>) {
        c[i + j] += a[i] * b[j];
      } else {
        unsigned int prod =
            (unsigned int)((unsigned long long)StaticModInt<m>(a[i]).val() *
                           (unsigned long long)StaticModInt<m>(b[j]).val() %
                           (unsigned int)m);
        unsigned int cur = (unsigned int)c[i + j];
        unsigned int sum = cur + prod;
        if (sum >= (unsigned int)m) sum -= (unsigned int)m;
        c[i + j] = (T)sum;
      }
    }
  }
  return c;
}

template <int m> void test_modint_pair(std::mt19937_64& rng, int n, int k) {
  std::uniform_int_distribution<int> dist(0, m - 1);
  std::vector<StaticModInt<m>> a(n), b(k);
  for (int i = 0; i < n; ++i) a[i] = StaticModInt<m>::raw(dist(rng));
  for (int i = 0; i < k; ++i) b[i] = StaticModInt<m>::raw(dist(rng));
  assert(convolution(a, b) == naive_convolution_mod<m>(a, b));
  if (n <= 1024) assert(convolution(a, a) == naive_convolution_mod<m>(a, a));
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
  std::vector<T> got = convolution<m>(a, b);
  std::vector<T> expected = naive_convolution_mod<m>(a, b);
  assert(got == expected);
  if (n <= 128) {
    assert(convolution<m>(a, a) == naive_convolution_mod<m>(a, a));
  }
}

inline void test_empty_and_small() {
  using Mint = StaticModInt<998244353>;
  assert(convolution(std::vector<Mint>{}, std::vector<Mint>{}).empty());
  assert(convolution(std::vector<Mint>{Mint(1)}, std::vector<Mint>{}).empty());
  assert(convolution(std::vector<Mint>{}, std::vector<Mint>{Mint(2)}).empty());

  assert(convolution(std::vector<int>{}, std::vector<int>{}).empty());
  assert(convolution(std::vector<int>{1}, std::vector<int>{}).empty());
  assert(convolution(std::vector<int>{}, std::vector<int>{2}).empty());

  // m = 2 (maximum size 1)
  assert((convolution(std::vector<StaticModInt<2>>{StaticModInt<2>(1)},
                      std::vector<StaticModInt<2>>{StaticModInt<2>(1)}) ==
          std::vector<StaticModInt<2>>{StaticModInt<2>(1)}));
  assert((convolution<2>(std::vector<int>{3}, std::vector<int>{-5}) ==
          std::vector<int>{1}));
  assert((convolution<2>(std::vector<bool>{true}, std::vector<bool>{true}) ==
          std::vector<bool>{true}));

  // m = 3 (maximum size 2)
  assert((convolution(std::vector<StaticModInt<3>>{StaticModInt<3>(2)},
                      std::vector<StaticModInt<3>>{StaticModInt<3>(2)}) ==
          std::vector<StaticModInt<3>>{StaticModInt<3>(1)}));
  assert((convolution<3>(std::vector<int>{1, 2}, std::vector<int>{2}) ==
          std::vector<int>{2, 1}));
}

inline void test_default_mod_and_types(std::mt19937_64& rng) {
  std::vector<int> ai = {-1, 0, 1, 998244353, 1000000000};
  std::vector<int> bi = {2, -3, 998244352};
  assert(convolution(ai, bi) == naive_convolution_mod<998244353>(ai, bi));

  for (int n : {1, 2, 15, 60, 61, 130}) {
    int k = n == 130 ? 70 : n + 3;
    test_integer_pair<257, int>(rng, n, k);
    test_integer_pair<257, unsigned int>(rng, n, k);
    test_integer_pair<257, long long>(rng, n, k);
    test_integer_pair<257, unsigned long long>(rng, n, k);
    test_integer_pair<257, __int128>(rng, n, k);
    test_integer_pair<257, unsigned __int128>(rng, n, k);
    test_integer_pair<257, short>(rng, n, k);
    test_integer_pair<257, unsigned short>(rng, n, k);
  }

  for (int n : {1, 8, 60, 61, 64}) {
    test_integer_pair<17, signed char>(rng, 8, 8);
    test_integer_pair<17, unsigned char>(rng, 8, 8);
    test_integer_pair<17, char>(rng, 8, 8);
    (void)n;
  }

  // bool with m = 2
  assert((convolution<2>(std::vector<bool>{false}, std::vector<bool>{true}) ==
          std::vector<bool>{false}));
}

inline void test_various_primes_and_sizes(std::mt19937_64& rng) {
  const std::pair<int, int> sizes[] = {
      {1, 1},     {1, 100},   {60, 60},   {60, 128},   {61, 61},   {64, 64},
      {65, 65},   {70, 200},  {200, 70},  {128, 128},  {129, 129}, {256, 256},
      {257, 257}, {500, 520}, {8200, 61}, {16400, 61},
  };
  for (auto [n, k] : sizes) {
    test_modint_pair<998244353>(rng, n, k);
    test_modint_pair<167772161>(rng, n, k);
    test_modint_pair<469762049>(rng, n, k);
    test_modint_pair<754974721>(rng, n, k);
    // Strict mode (m >= 2^30)
    test_modint_pair<1711276033>(rng, n, k);
    test_modint_pair<1811939329>(rng, n, k);
  }

  // Exact power-of-2 boundary for m = 257 (rank2 = 8, max len = 256)
  test_modint_pair<257>(rng, 128, 129);
  test_modint_pair<257>(rng, 61, 196);
  test_modint_pair<17>(rng, 8, 9);

  // Exact m = 998244353 boundary values (all elements = m - 1)
  {
    using Mint = StaticModInt<998244353>;
    std::vector<Mint> a(128, Mint(998244352));
    std::vector<Mint> b(128, Mint(998244352));
    assert(convolution(a, b) == naive_convolution_mod<998244353>(a, b));
  }
  // Exact m = 1811939329 boundary values (all elements = m - 1)
  {
    using Mint = StaticModInt<1811939329>;
    std::vector<Mint> a(128, Mint(1811939328));
    std::vector<Mint> b(128, Mint(1811939328));
    assert(convolution(a, b) == naive_convolution_mod<1811939329>(a, b));
  }
}

inline void run_all_convolution_tests() {
  std::mt19937_64 rng(0x9e3779b97f4a7c15ULL);
  test_empty_and_small();
  test_default_mod_and_types(rng);
  test_various_primes_and_sizes(rng);
}

}  // namespace

#endif  // LIBRARY3372_TESTS_MATH_CONVOLUTION_TEST_COMMON_HPP_
