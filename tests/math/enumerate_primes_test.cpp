// Repository: Library3372
// File: tests/math/enumerate_primes_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/math/enumerate_primes_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include "library3372/math/enumerate_primes.hpp"

#include <algorithm>
#include <cassert>
#include <climits>
#include <random>
#include <vector>

#include "library3372/math/is_prime.hpp"

namespace {

using library3372::enumerate_primes;
using library3372::is_prime;

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
  if (!enumerate_primes(0LL, 0LL).empty()) return false;
  if (!enumerate_primes(2LL, 2LL).empty()) return false;
  if (enumerate_primes(0LL, 30LL) !=
      (std::vector<long long>{2, 3, 5, 7, 11, 13, 17, 19, 23, 29})) {
    return false;
  }
  if (enumerate_primes(10LL, 32LL) !=
      (std::vector<long long>{11, 13, 17, 19, 23, 29, 31})) {
    return false;
  }
  if (enumerate_primes(288LL, 295LL) != std::vector<long long>{293}) {
    return false;
  }
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

std::vector<long long> reference_range(long long l, long long r) {
  std::vector<long long> res;
  for (long long x = l; x < r; ++x) {
    if (is_prime(x)) res.push_back(x);
  }
  return res;
}

void test_exhaustive_small() {
  static_assert(test_constexpr());

  for (int n = 0; n <= 500; ++n) {
    assert(enumerate_primes(n) == naive_sieve(n));
  }

  for (int l = 0; l <= 150; ++l) {
    for (int r = l; r <= 150; ++r) {
      assert(enumerate_primes((long long)l, (long long)r) ==
             reference_range(l, r));
    }
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

  // Range queries spanning boundaries and multi-segment intervals
  assert(enumerate_primes(960900LL, 961050LL) ==
         reference_range(960900LL, 961050LL));
  {
    auto it_l = std::lower_bound(ref.begin(), ref.end(), 25000);
    auto it_r = std::lower_bound(ref.begin(), ref.end(), 1000000);
    std::vector<long long> expected(it_l, it_r);
    assert(enumerate_primes(25000LL, 1000000LL) == expected);
  }

  // Around large prime threshold (16016^2 = 256512256)
  assert(enumerate_primes(256500000LL, 256530000LL) ==
         reference_range(256500000LL, 256530000LL));

  // Segmented sieve with large base primes (r = 2 * 10^12, len = 50000 ->
  // limit = 1414213 <= 50000 * 32)
  assert(enumerate_primes(1999999950000LL, 2000000000000LL) ==
         reference_range(1999999950000LL, 2000000000000LL));

  // is_prime fallback path (r = 10^13, len = 5000 -> limit > 5000 * 32)
  assert(enumerate_primes(9999999995000LL, 10000000000000LL) ==
         reference_range(9999999995000LL, 10000000000000LL));

  // Near LLONG_MAX
  assert(enumerate_primes(LLONG_MAX - 5000LL, LLONG_MAX) ==
         reference_range(LLONG_MAX - 5000LL, LLONG_MAX));
  assert(enumerate_primes(LLONG_MAX, LLONG_MAX).empty());
}

void test_random(std::mt19937_64& rng) {
  std::uniform_int_distribution<int> small_n(0, 100000);
  for (int i = 0; i < 20; ++i) {
    int n = small_n(rng);
    assert(enumerate_primes(n) == naive_sieve(n));
  }

  std::uniform_int_distribution<long long> med_l(0LL, 1000000000LL);
  std::uniform_int_distribution<long long> med_len(0LL, 5000LL);
  for (int i = 0; i < 30; ++i) {
    long long l = med_l(rng);
    long long r = l + med_len(rng);
    assert(enumerate_primes(l, r) == reference_range(l, r));
  }

  std::uniform_int_distribution<long long> large_l(1000000000000LL,
                                                   1000000000000000LL);
  std::uniform_int_distribution<long long> large_len(0LL, 2000LL);
  for (int i = 0; i < 20; ++i) {
    long long l = large_l(rng);
    long long r = l + large_len(rng);
    assert(enumerate_primes(l, r) == reference_range(l, r));
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
