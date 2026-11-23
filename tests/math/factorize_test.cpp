// Repository: Library3372
// File: tests/math/factorize_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/math/factorize_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include "library3372/math/factorize.hpp"

#include <cassert>
#include <climits>
#include <random>
#include <utility>
#include <vector>

#include "library3372/math/is_prime.hpp"

namespace {

using library3372::factorize;
using library3372::is_prime;

constexpr bool eq_factors(const std::vector<std::pair<long long, int>>& a,
                          const std::vector<std::pair<long long, int>>& b) {
  return a == b;
}

constexpr bool test_constexpr() {
  if (!eq_factors(factorize(1), {})) return false;
  if (!eq_factors(factorize(2), {{2, 1}})) return false;
  if (!eq_factors(factorize(12), {{2, 2}, {3, 1}})) return false;
  if (!eq_factors(factorize(360), {{2, 3}, {3, 2}, {5, 1}})) return false;
  if (!eq_factors(factorize(1369), {{37, 2}})) return false;
  if (!eq_factors(factorize(32767), {{7, 1}, {31, 1}, {151, 1}})) return false;
  if (!eq_factors(factorize(32771), {{32771, 1}})) return false;
  if (!eq_factors(factorize(32777), {{73, 1}, {449, 1}})) return false;
  if (!eq_factors(factorize(998244353), {{998244353, 1}})) return false;
  if (!eq_factors(factorize(998244352), {{2, 23}, {7, 1}, {17, 1}})) {
    return false;
  }
  if (!eq_factors(factorize(998244359987710471LL),
                  {{998244353LL, 1}, {1000000007LL, 1}})) {
    return false;
  }
  if (!eq_factors(factorize(4611686014132420609LL), {{2147483647LL, 2}})) {
    return false;
  }
  return true;
}

std::vector<std::pair<long long, int>> trial_factorize(long long n) {
  std::vector<std::pair<long long, int>> res;
  for (long long d = 2; d <= n / d; ++d) {
    if (n % d == 0) {
      int e = 0;
      while (n % d == 0) {
        n /= d;
        ++e;
      }
      res.emplace_back(d, e);
    }
  }
  if (n > 1) res.emplace_back(n, 1);
  return res;
}

void verify_factorization(long long n) {
  auto f = factorize(n);
  unsigned __int128 prod = 1;
  long long prev = 0;
  for (const auto& [p, e] : f) {
    assert(p > prev);
    assert(e >= 1);
    assert(is_prime(p));
    for (int i = 0; i < e; ++i) prod *= (unsigned long long)p;
    prev = p;
  }
  assert(prod == (unsigned long long)n);
}

void test_exhaustive_small() {
  static_assert(test_constexpr());

  for (long long n = 1; n <= 10000; ++n) {
    assert(factorize(n) == trial_factorize(n));
  }
}

void test_special_cases() {
  // Powers of 2
  for (int k = 1; k <= 62; ++k) {
    long long n = 1LL << k;
    assert(factorize(n) == (std::vector<std::pair<long long, int>>{{2, k}}));
    if (n - 1 >= 1) verify_factorization(n - 1);
    if (n < (1LL << 62)) verify_factorization(n + 1);
  }

  // Powers of small primes
  for (long long p : {3LL, 5LL, 7LL, 11LL, 13LL, 29LL, 31LL, 37LL, 41LL}) {
    long long cur = p;
    int e = 1;
    while (true) {
      assert(factorize(cur) ==
             (std::vector<std::pair<long long, int>>{{p, e}}));
      if (cur > LLONG_MAX / p) break;
      cur *= p;
      ++e;
    }
  }

  // Boundary around trial-division threshold (2^15 = 32768)
  for (long long n = 32750; n <= 32790; ++n) {
    assert(factorize(n) == trial_factorize(n));
  }

  // Carmichael numbers & strong pseudoprimes
  const long long special[] = {
      561LL,
      1105LL,
      1729LL,
      2465LL,
      2821LL,
      6601LL,
      8911LL,
      41041LL,
      4759123141LL,
      341550071728321LL,
      3825123056546413051LL,
  };
  for (long long x : special) verify_factorization(x);

  // Large primes and near LLONG_MAX
  verify_factorization((1LL << 61) - 1);
  verify_factorization(9223372036854775783LL);
  for (long long x = LLONG_MAX - 20; x < LLONG_MAX; ++x) {
    verify_factorization(x);
  }
  verify_factorization(LLONG_MAX);

  // Highly composite / product of many primes
  long long primorial = 1;
  for (long long p : {2LL, 3LL, 5LL, 7LL, 11LL, 13LL, 17LL, 19LL, 23LL, 29LL,
                      31LL, 37LL, 41LL, 43LL, 47LL}) {
    primorial *= p;
  }
  verify_factorization(primorial);
}

void test_random_and_semiprimes(std::mt19937_64& rng) {
  std::uniform_int_distribution<long long> med_dist(10001LL, 1000000000LL);
  for (int i = 0; i < 300; ++i) {
    long long n = med_dist(rng);
    assert(factorize(n) == trial_factorize(n));
  }

  std::uniform_int_distribution<long long> full_dist(1LL, LLONG_MAX);
  for (int i = 0; i < 500; ++i) {
    verify_factorization(full_dist(rng));
  }

  // Large semiprimes and prime squares near sqrt(LLONG_MAX)
  std::uniform_int_distribution<long long> p_dist(1000000000LL, 3037000499LL);
  std::vector<long long> primes;
  primes.reserve(20);
  while ((int)primes.size() < 20) {
    long long p = p_dist(rng) | 1LL;
    while (p <= 3037000499LL && !is_prime(p)) p += 2;
    if (p <= 3037000499LL) primes.push_back(p);
  }
  for (int i = 0; i < (int)primes.size(); ++i) {
    verify_factorization(primes[i] * primes[i]);
    for (int j = i + 1; j < (int)primes.size(); ++j) {
      verify_factorization(primes[i] * primes[j]);
    }
  }
}

}  // namespace

int main() {
  test_exhaustive_small();
  test_special_cases();

  std::mt19937_64 rng(0x517cc1b727220a95ULL);
  test_random_and_semiprimes(rng);
  return 0;
}
