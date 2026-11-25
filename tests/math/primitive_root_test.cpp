// Repository: Library3372
// File: tests/math/primitive_root_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/math/primitive_root_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include "library3372/math/primitive_root.hpp"

#include <cassert>
#include <climits>
#include <random>
#include <vector>

#include "library3372/math/factorize.hpp"
#include "library3372/math/is_prime.hpp"

namespace {

using library3372::factorize;
using library3372::is_prime;
using library3372::primitive_root;

constexpr long long mod_pow_128(long long base, long long exp, long long mod) {
  unsigned long long res = 1;
  unsigned long long b = (unsigned long long)(base % mod);
  unsigned long long u = (unsigned long long)mod;
  unsigned long long e = (unsigned long long)exp;
  while (e > 0) {
    if (e & 1ULL) res = (unsigned long long)((unsigned __int128)res * b % u);
    b = (unsigned long long)((unsigned __int128)b * b % u);
    e >>= 1;
  }
  return (long long)res;
}

constexpr bool verify_constexpr(long long p) {
  long long g = primitive_root(p);
  if (p == 2) return g == 1;
  if (g < 2 || g >= p) return false;
  auto factors = factorize(p - 1);
  auto is_prim = [&](long long x) {
    for (const auto& [q, e] : factors) {
      if (mod_pow_128(x, (p - 1) / q, p) == 1) return false;
    }
    return true;
  };
  if (!is_prim(g)) return false;
  for (long long x = 2; x < g; ++x) {
    if (is_prim(x)) return false;
  }
  return true;
}

constexpr bool test_constexpr() {
  if (primitive_root(2) != 1) return false;
  if (primitive_root(3) != 2) return false;
  if (primitive_root(5) != 2) return false;
  if (primitive_root(7) != 3) return false;
  if (primitive_root(11) != 2) return false;
  if (primitive_root(41) != 6) return false;
  if (primitive_root(71) != 7) return false;
  if (primitive_root(191) != 19) return false;
  if (primitive_root(409) != 21) return false;
  if (primitive_root(998244353) != 3) return false;
  if (primitive_root(1000000007) != 5) return false;
  if (!verify_constexpr(2147483647LL)) return false;
  if (!verify_constexpr(2305843009213693951LL)) return false;
  return true;
}

long long naive_primitive_root(long long p) {
  if (p == 2) return 1;
  for (long long g = 2; g < p; ++g) {
    long long cur = g % p;
    long long ord = 1;
    while (cur != 1) {
      cur = (cur * g) % p;
      ++ord;
    }
    if (ord == p - 1) return g;
  }
  assert(false);
  return -1;
}

void verify_primitive_root(long long p) {
  long long g = primitive_root(p);
  if (p == 2) {
    assert(g == 1);
    return;
  }
  assert(2 <= g && g < p);
  auto factors = factorize(p - 1);
  auto is_prim = [&](long long x) {
    for (const auto& [q, e] : factors) {
      if (mod_pow_128(x, (p - 1) / q, p) == 1) return false;
    }
    return true;
  };
  assert(is_prim(g));
  for (long long x = 2; x < g; ++x) assert(!is_prim(x));
}

void test_small_exhaustive() {
  static_assert(test_constexpr());

  for (long long p = 2; p <= 2000; ++p) {
    if (!is_prime(p)) continue;
    assert(primitive_root(p) == naive_primitive_root(p));
  }
}

void test_special_cases() {
  // Known small and NTT-friendly primes
  assert(primitive_root(109) == 6);
  assert(primitive_root(167772161) == 3);
  assert(primitive_root(469762049) == 3);
  assert(primitive_root(754974721) == 11);
  assert(primitive_root(1224736769) == 3);
  assert(primitive_root(2113929217) == 5);
  assert(primitive_root(2130706433) == 3);
  assert(primitive_root(2147483647) == 7);

  // Primes with relatively large smallest primitive roots
  const long long large_root_primes[] = {
      191LL, 409LL, 2111LL, 13441LL, 71147LL, 179057LL, 360229LL,
  };
  for (long long p : large_root_primes) {
    assert(is_prime(p));
    verify_primitive_root(p);
  }

  // 64-bit primes including max distinct prime factors (15) in p - 1,
  // Mersenne prime 2^61 - 1, 64-bit NTT primes, and primes near LLONG_MAX.
  const long long large_primes[] = {
      8608456956238879741LL,  // 1 + 14 * (2 * 3 * ... * 47)
      2305843009213693951LL,  // 2^61 - 1
      4179340454199820289LL, 1945555039024054273LL,
      9223372036854775783LL,  // Largest prime <= LLONG_MAX
  };
  for (long long p : large_primes) {
    assert(is_prime(p));
    verify_primitive_root(p);
  }

  for (long long p = LLONG_MAX; p >= LLONG_MAX - 200; p -= 2) {
    if (is_prime(p)) verify_primitive_root(p);
  }
}

void test_random(std::mt19937_64& rng) {
  auto next_prime = [&](long long x, long long max_val) {
    x |= 1LL;
    while (x <= max_val && !is_prime(x)) x += 2;
    return x <= max_val ? x : -1;
  };

  std::uniform_int_distribution<long long> dist32(2001LL, 2147483647LL);
  int cnt32 = 0;
  while (cnt32 < 100) {
    long long p = next_prime(dist32(rng), 2147483647LL);
    if (p != -1) {
      verify_primitive_root(p);
      ++cnt32;
    }
  }

  std::uniform_int_distribution<long long> dist64(1000000000000000000LL,
                                                  LLONG_MAX);
  int cnt64 = 0;
  while (cnt64 < 100) {
    long long p = next_prime(dist64(rng), LLONG_MAX);
    if (p != -1) {
      verify_primitive_root(p);
      ++cnt64;
    }
  }
}

}  // namespace

int main() {
  test_small_exhaustive();
  test_special_cases();

  std::mt19937_64 rng(0x7a3b9c1d5e2f4801ULL);
  test_random(rng);
  return 0;
}
