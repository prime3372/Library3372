// Repository: Library3372
// File: tests/math/convolution_mod_2_32_test_common.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/math/convolution_mod_2_32_test_common.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_TESTS_MATH_CONVOLUTION_MOD_2_32_TEST_COMMON_HPP_
#define LIBRARY3372_TESTS_MATH_CONVOLUTION_MOD_2_32_TEST_COMMON_HPP_

#include <cassert>
#include <climits>
#include <random>
#include <utility>
#include <vector>

#include "library3372/math/convolution_mod_2_32.hpp"

namespace {

using library3372::convolution_mod_2_32;

template <typename T>
std::vector<T> naive_convolution_mod_2_32(const std::vector<T>& a,
                                          const std::vector<T>& b) {
  if (a.empty() || b.empty()) return {};
  int n = (int)a.size();
  int k = (int)b.size();
  std::vector<T> c(n + k - 1);
  for (int i = 0; i < n; ++i) {
    unsigned int av = (unsigned int)a[i];
    for (int j = 0; j < k; ++j) {
      unsigned int cur = (unsigned int)c[i + j];
      c[i + j] = (T)(cur + av * (unsigned int)b[j]);
    }
  }
  return c;
}

template <typename T>
void test_mod_2_32_pair(std::mt19937_64& rng, int n, int k) {
  std::vector<T> a(n), b(k);
  for (int i = 0; i < n; ++i) a[i] = (T)rng();
  for (int i = 0; i < k; ++i) b[i] = (T)rng();
  assert(convolution_mod_2_32(a, b) == naive_convolution_mod_2_32(a, b));
  if (n <= 256) {
    assert(convolution_mod_2_32(a, a) == naive_convolution_mod_2_32(a, a));
  }
}

inline void run_convolution_mod_2_32_tests() {
  assert(convolution_mod_2_32(std::vector<unsigned int>{},
                              std::vector<unsigned int>{})
             .empty());
  assert(convolution_mod_2_32(std::vector<unsigned int>{1U},
                              std::vector<unsigned int>{})
             .empty());
  assert(convolution_mod_2_32(std::vector<unsigned int>{},
                              std::vector<unsigned int>{2U})
             .empty());

  std::vector<unsigned int> edge_a(200, UINT_MAX);
  std::vector<unsigned int> edge_b(200, UINT_MAX);
  assert(convolution_mod_2_32(edge_a, edge_b) ==
         naive_convolution_mod_2_32(edge_a, edge_b));

  std::vector<int> neg_a(100, INT_MIN);
  std::vector<int> neg_b(100, -1);
  assert(convolution_mod_2_32(neg_a, neg_b) ==
         naive_convolution_mod_2_32(neg_a, neg_b));

  std::mt19937_64 rng(0x24681357fedcba98ULL);
  for (auto [n, k] : std::vector<std::pair<int, int>>{{1, 1},
                                                      {1, 100},
                                                      {60, 60},
                                                      {60, 128},
                                                      {61, 61},
                                                      {65, 130},
                                                      {128, 128},
                                                      {256, 257},
                                                      {512, 513},
                                                      {1024, 1024}}) {
    test_mod_2_32_pair<unsigned int>(rng, n, k);
    if (n <= 257) {
      test_mod_2_32_pair<int>(rng, n, k);
      test_mod_2_32_pair<long long>(rng, n, k);
      test_mod_2_32_pair<unsigned long long>(rng, n, k);
      test_mod_2_32_pair<__int128>(rng, n, k);
      test_mod_2_32_pair<bool>(rng, n, k);
    }
  }
}

}  // namespace

#endif  // LIBRARY3372_TESTS_MATH_CONVOLUTION_MOD_2_32_TEST_COMMON_HPP_
