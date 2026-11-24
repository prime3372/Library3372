// Repository: Library3372
// File: tests/math/enumerate_primes_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/math/enumerate_primes_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include "library3372/math/enumerate_primes.hpp"

#include <algorithm>
#include <cassert>
#include <random>
#include <vector>

namespace {

using library3372::enumerate_primes;

constexpr bool test_constexpr() {
  if (!enumerate_primes(0).empty()) return false;
  if (!enumerate_primes(1).empty()) return false;
  if (enumerate_primes(2) != std::vector<int>{2}) return false;
  if (enumerate_primes(3) != (std::vector<int>{2, 3})) return false;
  if (enumerate_primes(5) != (std::vector<int>{2, 3, 5})) return false;
  if (enumerate_primes(30) !=
      (std::vector<int>{2, 3, 5, 7, 11, 13, 17, 19, 23, 29})) {
    return false;
  }
  if (enumerate_primes(31) !=
      (std::vector<int>{2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31})) {
    return false;
  }
  if (enumerate_primes(295).back() != 293) return false;
  return true;
}

std::vector<int> naive_sieve(int n) {
  if (n < 2) return {};
  std::vector<bool> is_p(n + 1, true);
  is_p[0] = false;
  is_p[1] = false;
  for (int i = 2; (long long)i * i <= n; ++i) {
    if (!is_p[i]) continue;
    for (int j = i * i; j <= n; j += i) is_p[j] = false;
  }
  std::vector<int> res;
  for (int i = 2; i <= n; ++i) {
    if (is_p[i]) res.push_back(i);
  }
  return res;
}

void test_exhaustive_small() {
  static_assert(test_constexpr());

  for (int n = 0; n <= 500; ++n) {
    assert(enumerate_primes(n) == naive_sieve(n));
  }
}

void test_boundaries_and_multi_segment() {
  // Boundaries around pattern period (30030), wheel period (30),
  // and segment size (32032 * 30 = 960960)
  const int boundaries[] = {
      288,    289,    290,    30029,  30030,   30031,
      960959, 960960, 960961, 960990, 2000000,
  };
  const std::vector<int> ref = naive_sieve(2000000);
  assert(enumerate_primes(2000000) == ref);

  for (int n : boundaries) {
    auto got = enumerate_primes(n);
    auto it = std::upper_bound(ref.begin(), ref.end(), n);
    std::vector<int> expected(ref.begin(), it);
    assert(got == expected);
  }
}

void test_random(std::mt19937_64& rng) {
  std::uniform_int_distribution<int> small_n(0, 100000);
  for (int i = 0; i < 20; ++i) {
    int n = small_n(rng);
    assert(enumerate_primes(n) == naive_sieve(n));
  }
}

}  // namespace

int main() {
  test_exhaustive_small();
  test_boundaries_and_multi_segment();

  std::mt19937_64 rng(0x243f6a8885a308d3ULL);
  test_random(rng);
  return 0;
}
