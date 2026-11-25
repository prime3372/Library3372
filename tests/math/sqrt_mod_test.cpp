// Repository: Library3372
// File: tests/math/sqrt_mod_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/math/sqrt_mod_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include "library3372/math/sqrt_mod.hpp"

#include <algorithm>
#include <cassert>
#include <climits>
#include <optional>
#include <random>
#include <vector>

#include "library3372/math/is_prime.hpp"

namespace {

using library3372::is_prime;
using library3372::sqrt_mod;

constexpr long long norm_mod(long long a, long long p) {
  long long r = a % p;
  return r < 0 ? r + p : r;
}

constexpr long long pow_mod_128(long long a, long long e, long long p) {
  unsigned __int128 res = 1;
  unsigned __int128 base = (unsigned long long)norm_mod(a, p);
  unsigned long long exp = (unsigned long long)e;
  unsigned long long mod = (unsigned long long)p;
  while (exp > 0) {
    if (exp & 1ULL) res = (res * base) % mod;
    base = (base * base) % mod;
    exp >>= 1;
  }
  return (long long)res;
}

constexpr bool check_sqrt(long long a, long long p,
                          std::optional<long long> expected) {
  auto got = sqrt_mod(a, p);
  if (got != expected) return false;
  if (got.has_value()) {
    if (*got < 0 || *got > p / 2) return false;
    unsigned __int128 sq =
        (unsigned __int128)(*got) * (unsigned __int128)(*got);
    if ((long long)(sq % (unsigned long long)p) != norm_mod(a, p)) return false;
  }
  return true;
}

constexpr bool test_constexpr() {
  if (!check_sqrt(0, 2, 0)) return false;
  if (!check_sqrt(1, 2, 1)) return false;
  if (!check_sqrt(-1, 2, 1)) return false;
  if (!check_sqrt(LLONG_MIN, 2, 0)) return false;
  if (!check_sqrt(LLONG_MAX, 2, 1)) return false;

  if (!check_sqrt(0, 3, 0)) return false;
  if (!check_sqrt(1, 3, 1)) return false;
  if (!check_sqrt(2, 3, std::nullopt)) return false;
  if (!check_sqrt(-2, 3, 1)) return false;

  if (!check_sqrt(0, 5, 0)) return false;
  if (!check_sqrt(1, 5, 1)) return false;
  if (!check_sqrt(2, 5, std::nullopt)) return false;
  if (!check_sqrt(3, 5, std::nullopt)) return false;
  if (!check_sqrt(4, 5, 2)) return false;

  if (!check_sqrt(5, 11, 4)) return false;
  if (!check_sqrt(2, 11, std::nullopt)) return false;

  if (!check_sqrt(2, 17, 6)) return false;
  if (!check_sqrt(3, 17, std::nullopt)) return false;

  // 32-bit primes: s = 23 and s = 1
  if (!check_sqrt(7, 998244353LL, 116190042LL)) return false;
  if (!check_sqrt(3, 998244353LL, std::nullopt)) return false;
  if (!check_sqrt(-1, 1000000007LL, std::nullopt)) return false;
  if (!check_sqrt(4, 1000000007LL, 2)) return false;

  // 64-bit primes
  constexpr long long p61 = (1LL << 61) - 1;
  if (!check_sqrt(1000000000000000000LL, p61, 1000000000LL)) return false;
  if (!check_sqrt(-1, p61, std::nullopt)) return false;

  constexpr long long p64 = 9223372036854775783LL;
  if (!check_sqrt(-1, p64, std::nullopt)) return false;
  if (!check_sqrt(9, p64, 3)) return false;

  // 64-bit prime with large 2-adic valuation: p = 29 * 2^57 + 1
  constexpr long long p_proth = (29LL << 57) + 1;
  if (!is_prime(p_proth)) return false;
  if (!check_sqrt(4, p_proth, 2)) return false;
  if (!check_sqrt(16, p_proth, 4)) return false;
  if (!check_sqrt(3, p_proth, std::nullopt)) return false;

  // s >= 3 with t == 1 (10 = 16^2 mod 41, 10^5 == 1 mod 41)
  if (!check_sqrt(10, 41, 16)) return false;
  return true;
}

void verify_with_euler(long long a, long long p) {
  auto res = sqrt_mod(a, p);
  long long rem = norm_mod(a, p);
  if (rem == 0) {
    assert(res.has_value() && *res == 0);
    return;
  }
  if (p == 2) {
    assert(res.has_value() && *res == 1);
    return;
  }
  long long leg = pow_mod_128(rem, (p - 1) >> 1, p);
  if (leg == 1) {
    assert(res.has_value());
    assert(0 <= *res && *res <= p / 2);
    unsigned __int128 sq =
        (unsigned __int128)(*res) * (unsigned __int128)(*res);
    assert((long long)(sq % (unsigned long long)p) == rem);
  } else {
    assert(leg == p - 1);
    assert(!res.has_value());
  }
}

void test_exhaustive_small() {
  static_assert(test_constexpr());

  for (long long p = 2; p <= 200; ++p) {
    if (!is_prime(p)) continue;
    std::vector<std::optional<long long>> expected((int)p);
    for (int x = 0; x < p; ++x) {
      int sq = (x * x) % (int)p;
      if (!expected[sq].has_value()) expected[sq] = x;
    }
    for (long long a = -2 * p; a <= 2 * p; ++a) {
      int rem = (int)norm_mod(a, p);
      assert(sqrt_mod(a, p) == expected[rem]);
    }
  }
}

void test_special_primes(std::mt19937_64& rng) {
  // Include primes with various s = v2(p - 1) and large least QNR
  const long long primes[] = {
      2LL,
      3LL,
      5LL,
      7LL,
      13LL,
      17LL,
      41LL,
      73LL,
      97LL,
      193LL,
      257LL,
      433LL,
      1321LL,
      4729LL,
      65537LL,
      998244353LL,
      1000000007LL,
      (1LL << 31) - 1,
      (1LL << 61) - 1,
      (29LL << 57) + 1,
      (27LL << 56) + 1,
      9223372036854775783LL,
  };

  for (long long p : primes) {
    assert(is_prime(p));
    const long long special_a[] = {
        LLONG_MIN,
        LLONG_MIN + 1,
        -p - 1,
        -p,
        -p + 1,
        -2,
        -1,
        0,
        1,
        2,
        3,
        p - 1,
        p,
        p + 1,
        LLONG_MAX - 1,
        LLONG_MAX,
    };
    for (long long a : special_a) verify_with_euler(a, p);

    std::uniform_int_distribution<long long> dist_x(0, p - 1);
    std::uniform_int_distribution<long long> dist_all(LLONG_MIN, LLONG_MAX);
    for (int i = 0; i < 100; ++i) {
      long long x = dist_x(rng);
      unsigned __int128 sq = (unsigned __int128)x * (unsigned __int128)x;
      long long a = (long long)(sq % (unsigned long long)p);
      long long expected = std::min(x, p - x);
      assert(sqrt_mod(a, p) == expected);
      if (a > 0) assert(sqrt_mod(a - p, p) == expected);
      if (a <= LLONG_MAX - p) assert(sqrt_mod(a + p, p) == expected);

      verify_with_euler(dist_all(rng), p);
    }
  }
}

void test_random_primes(std::mt19937_64& rng) {
  auto next_prime = [](long long x) {
    x |= 1LL;
    while (!is_prime(x)) x += 2;
    return x;
  };

  std::uniform_int_distribution<long long> p32_dist(100000000LL, 2000000000LL);
  std::uniform_int_distribution<long long> p64_dist(1000000000000000000LL,
                                                    9223372036854775000LL);
  for (int t = 0; t < 40; ++t) {
    long long p = next_prime(t < 20 ? p32_dist(rng) : p64_dist(rng));
    std::uniform_int_distribution<long long> dist_x(0, p - 1);
    for (int i = 0; i < 50; ++i) {
      long long x = dist_x(rng);
      unsigned __int128 sq = (unsigned __int128)x * (unsigned __int128)x;
      long long a = (long long)(sq % (unsigned long long)p);
      assert(sqrt_mod(a, p) == std::min(x, p - x));
      verify_with_euler(dist_x(rng), p);
    }
  }
}

}  // namespace

int main() {
  test_exhaustive_small();

  std::mt19937_64 rng(0x243f6a8885a308d3ULL);
  test_special_primes(rng);
  test_random_primes(rng);
  return 0;
}
