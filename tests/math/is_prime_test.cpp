// Repository: Library3372
// File: tests/math/is_prime_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/math/is_prime_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include "library3372/math/is_prime.hpp"

#include <cassert>
#include <climits>
#include <random>
#include <vector>

namespace {

using library3372::is_prime;

constexpr bool test_constexpr() {
  if (is_prime(LLONG_MIN)) return false;
  if (is_prime(-1000000007)) return false;
  if (is_prime(-1)) return false;
  if (is_prime(0)) return false;
  if (is_prime(1)) return false;
  if (!is_prime(2)) return false;
  if (!is_prime(3)) return false;
  if (is_prime(4)) return false;
  if (!is_prime(5)) return false;
  if (is_prime(561)) return false;
  if (!is_prime(998244353)) return false;
  if (!is_prime(1000000007)) return false;
  if (is_prime(341550071728321LL)) return false;
  if (!is_prime(2305843009213693951LL)) return false;
  if (!is_prime(9223372036854775783LL)) return false;
  if (is_prime(LLONG_MAX)) return false;
  return true;
}

bool trial_is_prime(long long n) {
  if (n <= 1) return false;
  if (n <= 3) return true;
  if (n % 2 == 0 || n % 3 == 0) return false;
  for (long long d = 5; d <= n / d; d += 6) {
    if (n % d == 0 || n % (d + 2) == 0) return false;
  }
  return true;
}

void test_boundaries_and_special_cases() {
  static_assert(test_constexpr());

  assert(!is_prime(LLONG_MIN));
  assert(!is_prime(LLONG_MIN + 1));
  assert(!is_prime(-1000000007));
  assert(!is_prime(-3));
  assert(!is_prime(-2));
  assert(!is_prime(-1));
  assert(!is_prime(0));
  assert(!is_prime(1));

  // Miller-Rabin bases, filter primes, and their prime factors / neighbors
  const long long base_related[] = {
      2,   3,    5,     7,      11,     13,      19,        73,        193,
      325, 9375, 28178, 407521, 450775, 9780504, 299210837, 1795265022};
  for (long long x : base_related) {
    for (long long d = -2; d <= 2; ++d) {
      assert(is_prime(x + d) == trial_is_prime(x + d));
    }
  }

  // Threshold of {2, 7, 61} bases (4759123141 is the smallest counterexample)
  assert(is_prime(4759123129LL));
  assert(!is_prime(4759123141LL));
  assert(is_prime(4759123151LL));

  // Carmichael numbers
  const long long carmichael[] = {561,  1105, 1729,  2465,  2821,
                                  6601, 8911, 41041, 62745, 63973};
  for (long long x : carmichael) assert(!is_prime(x));

  // Strong pseudoprimes to consecutive prime bases
  const long long sprp[] = {
      2047LL,
      1373653LL,
      25326001LL,
      3215031751LL,
      2152302898747LL,
      3474749660383LL,
      341550071728321LL,
      3825123056546413051LL,
  };
  for (long long x : sprp) assert(!is_prime(x));

  // Powers of two and Mersenne numbers
  assert(is_prime((1LL << 31) - 1));
  assert(!is_prime((1LL << 32) - 1));
  assert(!is_prime((1LL << 32) + 1));
  assert(is_prime((1LL << 32) + 15));
  assert(is_prime((1LL << 61) - 1));

  // Near LLONG_MAX
  assert(is_prime(9223372036854775783LL));
  for (long long x = 9223372036854775784LL; x < LLONG_MAX; ++x) {
    assert(!is_prime(x));
  }
  assert(!is_prime(LLONG_MAX));
}

void test_small_exhaustive() {
  constexpr int limit = 100000;
  std::vector<bool> sieve(limit + 1, true);
  sieve[0] = false;
  sieve[1] = false;
  for (int i = 2; (long long)i * i <= limit; ++i) {
    if (!sieve[i]) continue;
    for (int j = i * i; j <= limit; j += i) sieve[j] = false;
  }

  for (int x = -100; x <= limit; ++x) {
    bool expected = x >= 2 && sieve[x];
    assert(is_prime(x) == expected);
  }
}

void test_random_with_trial_division(std::mt19937_64& rng) {
  std::uniform_int_distribution<long long> med_dist(100001LL, 1000000000LL);
  for (int i = 0; i < 1000; ++i) {
    long long x = med_dist(rng);
    assert(is_prime(x) == trial_is_prime(x));
    assert(is_prime(x | 1LL) == trial_is_prime(x | 1LL));
  }

  std::uniform_int_distribution<long long> large_dist(1000000001LL,
                                                      100000000000LL);
  for (int i = 0; i < 100; ++i) {
    long long x = large_dist(rng) | 1LL;
    assert(is_prime(x) == trial_is_prime(x));
  }
}

void test_large_semiprimes(std::mt19937_64& rng) {
  // Floor(sqrt(LLONG_MAX)) = 3037000499
  std::uniform_int_distribution<long long> dist(1000000000LL, 3037000499LL);
  auto next_prime = [&](long long x) {
    x |= 1LL;
    while (!trial_is_prime(x)) x += 2;
    return x;
  };

  std::vector<long long> primes;
  primes.reserve(40);
  for (int i = 0; i < 40; ++i) {
    long long p = next_prime(dist(rng));
    if (p <= 3037000499LL) {
      assert(is_prime(p));
      primes.push_back(p);
    }
  }

  for (int i = 0; i < (int)primes.size(); ++i) {
    for (int j = i; j < (int)primes.size(); ++j) {
      long long prod = primes[i] * primes[j];
      assert(!is_prime(prod));
    }
  }
}

}  // namespace

int main() {
  test_boundaries_and_special_cases();
  test_small_exhaustive();

  std::mt19937_64 rng(0x9e3779b97f4a7c15ULL);
  test_random_with_trial_division(rng);
  test_large_semiprimes(rng);
  return 0;
}
