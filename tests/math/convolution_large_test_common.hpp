// Repository: Library3372
// File: tests/math/convolution_large_test_common.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/math/convolution_large_test_common.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_TESTS_MATH_CONVOLUTION_LARGE_TEST_COMMON_HPP_
#define LIBRARY3372_TESTS_MATH_CONVOLUTION_LARGE_TEST_COMMON_HPP_

#include <cassert>
#include <climits>
#include <random>
#include <utility>
#include <vector>

#include "library3372/math/convolution_large.hpp"
#include "library3372/math/dynamic_modint.hpp"
#include "library3372/math/static_modint.hpp"

namespace {

using library3372::convolution_large;
using library3372::DynamicModInt;
using library3372::StaticModInt;

template <typename T>
std::vector<T> naive_convolution_large(const std::vector<T>& a,
                                       const std::vector<T>& b, long long mod) {
  if (a.empty() || b.empty()) return {};
  int n = (int)a.size();
  int k = (int)b.size();
  std::vector<T> c(n + k - 1);
  unsigned long long umod = (unsigned long long)mod;
  auto norm = [mod](T v) -> unsigned long long {
    if constexpr (library3372::internal::is_signed_integer_v<T>) {
      long long r = (long long)(v % (__int128)mod);
      if (r < 0) r += mod;
      return (unsigned long long)r;
    } else if constexpr (library3372::internal::is_unsigned_integer_v<T>) {
      return (unsigned long long)(v % (unsigned __int128)mod);
    } else {
      return (unsigned long long)v.val();
    }
  };
  for (int i = 0; i < n; ++i) {
    unsigned long long av = norm(a[i]);
    for (int j = 0; j < k; ++j) {
      unsigned long long bv = norm(b[j]);
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

inline void run_convolution_large_tests() {
  using SMint = StaticModInt<1000000007>;
  assert(convolution_large(std::vector<SMint>{}, std::vector<SMint>{}).empty());
  assert(convolution_large(std::vector<int>{}, std::vector<int>{}, 1).empty());

  std::vector<SMint> m1_a(80, 5);
  std::vector<StaticModInt<1>> z1(80, 0);
  assert(convolution_large(z1, z1) == std::vector<StaticModInt<1>>(159, 0));

  std::mt19937_64 rng(0xabcdef0123456789ULL);
  for (auto [n, k] : std::vector<std::pair<int, int>>{
           {1, 1}, {1, 70}, {60, 60}, {61, 61}, {129, 130}, {256, 257}}) {
    {
      std::vector<SMint> a(n), b(k);
      for (int i = 0; i < n; ++i) a[i] = (long long)rng();
      for (int i = 0; i < k; ++i) b[i] = (long long)rng();
      assert(convolution_large(a, b) ==
             naive_convolution_large(a, b, 1000000007));
      assert(convolution_large(a, a) ==
             naive_convolution_large(a, a, 1000000007));
    }
    {
      using DMint = DynamicModInt<10>;
      DMint::set_mod(1000000000);
      std::vector<DMint> a(n), b(k);
      for (int i = 0; i < n; ++i) a[i] = (long long)rng();
      for (int i = 0; i < k; ++i) b[i] = (long long)rng();
      assert(convolution_large(a, b) ==
             naive_convolution_large(a, b, 1000000000));
    }
    {
      std::vector<int> a(n), b(k);
      for (int i = 0; i < n; ++i) a[i] = (int)rng();
      for (int i = 0; i < k; ++i) b[i] = (int)rng();
      assert(convolution_large<INT_MAX>(a, b) ==
             naive_convolution_large(a, b, INT_MAX));
    }
    {
      std::vector<long long> a(n), b(k);
      for (int i = 0; i < n; ++i) a[i] = (long long)rng();
      for (int i = 0; i < k; ++i) b[i] = (long long)rng();
      assert(convolution_large(a, b, LLONG_MAX) ==
             naive_convolution_large(a, b, LLONG_MAX));
      assert(convolution_large<1000000000000000007LL>(a, b) ==
             naive_convolution_large(a, b, 1000000000000000007LL));
    }
  }

  // Verify bool output path
  {
    std::vector<bool> ba = {true, false, true};
    std::vector<bool> bb = {true, true};
    assert(convolution_large<2>(ba, bb) == naive_convolution_large(ba, bb, 2));
  }

  // Verify length exceeding threshold cleanly
  {
    constexpr int n = 4096;
    constexpr int k = 4097;
    std::vector<SMint> a(n, 1);
    std::vector<SMint> b(k, 1);
    std::vector<SMint> c = convolution_large(a, b);
    assert((int)c.size() == n + k - 1);
    for (int i = 0; i < (int)c.size(); ++i) {
      int lo = std::max(0, i - (n - 1));
      int hi = std::min(k - 1, i);
      assert(c[i].val() == hi - lo + 1);
    }
  }
}

}  // namespace

#endif  // LIBRARY3372_TESTS_MATH_CONVOLUTION_LARGE_TEST_COMMON_HPP_
