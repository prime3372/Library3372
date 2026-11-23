// Repository: Library3372
// File: tests/math/convolution_ll_test_common.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/math/convolution_ll_test_common.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_TESTS_MATH_CONVOLUTION_LL_TEST_COMMON_HPP_
#define LIBRARY3372_TESTS_MATH_CONVOLUTION_LL_TEST_COMMON_HPP_

#include <cassert>
#include <random>
#include <utility>
#include <vector>

#include "library3372/math/convolution_ll.hpp"

namespace {

using library3372::convolution_ll;

inline std::vector<long long> naive_convolution_ll(
    const std::vector<long long>& a, const std::vector<long long>& b) {
  if (a.empty() || b.empty()) return {};
  int n = (int)a.size();
  int k = (int)b.size();
  std::vector<long long> c(n + k - 1, 0);
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < k; ++j) {
      c[i + j] =
          (long long)((unsigned long long)c[i + j] +
                      (unsigned long long)a[i] * (unsigned long long)b[j]);
    }
  }
  return c;
}

inline void test_convolution_ll_empty_and_small() {
  assert(convolution_ll(std::vector<long long>{}, std::vector<long long>{})
             .empty());
  assert(convolution_ll(std::vector<long long>{1}, std::vector<long long>{})
             .empty());
  assert(convolution_ll(std::vector<long long>{}, std::vector<long long>{2})
             .empty());
  assert((convolution_ll(std::vector<long long>{3, -4},
                         std::vector<long long>{5, 6}) ==
          std::vector<long long>{15, -2, -24}));
}

inline void test_convolution_ll_random_and_boundary(std::mt19937_64& rng) {
  const std::pair<int, int> sizes[] = {
      {1, 1},     {1, 80},    {60, 60},   {60, 150},  {61, 61},    {64, 65},
      {128, 129}, {256, 257}, {400, 450}, {8200, 61}, {16400, 61},
  };
  std::uniform_int_distribution<long long> dist(-100000000LL, 100000000LL);
  for (auto [n, k] : sizes) {
    std::vector<long long> a(n), b(k);
    for (int i = 0; i < n; ++i) a[i] = dist(rng);
    for (int i = 0; i < k; ++i) b[i] = dist(rng);
    assert(convolution_ll(a, b) == naive_convolution_ll(a, b));
    if (n <= 1024) assert(convolution_ll(a, a) == naive_convolution_ll(a, a));
  }

  // Extreme values within 2-prime CRT range with NTT path (n = k = 64)
  {
    constexpr long long limit = 2252081290784276480LL;
    std::vector<long long> a(64, 0), b(64, 0);
    a[0] = limit;
    b[0] = 1;
    a[63] = -limit;
    b[63] = 1;
    a[31] = -1;
    b[32] = 1;
    assert(convolution_ll(a, b) == naive_convolution_ll(a, b));
  }
  {
    std::vector<long long> a(65, 100000000LL), b(65, -100000000LL);
    assert(convolution_ll(a, b) == naive_convolution_ll(a, b));
  }
  // Cancellation from huge intermediate terms into CRT range
  {
    std::vector<long long> a(64, 1000000000000LL), b(64, 0);
    b[0] = 1000000LL;
    b[1] = -1000000LL;
    assert(convolution_ll(a, b) == naive_convolution_ll(a, b));
  }
}

inline void run_all_convolution_ll_tests() {
  std::mt19937_64 rng(0x9e3779b97f4a7c15ULL);
  test_convolution_ll_empty_and_small();
  test_convolution_ll_random_and_boundary(rng);
}

}  // namespace

#endif  // LIBRARY3372_TESTS_MATH_CONVOLUTION_LL_TEST_COMMON_HPP_
