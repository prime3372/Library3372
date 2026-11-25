// Repository: Library3372
// File: tests/math/floor_sum_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/math/floor_sum_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include "library3372/math/floor_sum.hpp"

#include <cassert>
#include <climits>
#include <random>
#include <vector>

namespace {

using library3372::floor_sum;

unsigned long long naive_floor_sum(long long n, long long m, long long a,
                                   long long b) {
  unsigned long long sum = 0;
  for (long long i = 0; i < n; ++i) {
    __int128 num = (__int128)a * i + b;
    __int128 q = num / m;
    if (num % m < 0) --q;
    sum += (unsigned long long)q;
  }
  return sum;
}

unsigned long long ref_floor_sum(__int128 n, __int128 m, __int128 a,
                                 __int128 b) {
  if (n == 0) return 0;
  unsigned long long ans = 0;
  auto div_floor = [](__int128 x, __int128 y) -> __int128 {
    __int128 q = x / y;
    if (x % y < 0) --q;
    return q;
  };
  if (a < 0 || a >= m) {
    __int128 q = div_floor(a, m);
    __int128 r = a - q * m;
    unsigned __int128 c2 =
        (unsigned __int128)n * (unsigned __int128)(n - 1) / 2;
    ans += (unsigned long long)c2 * (unsigned long long)q;
    a = r;
  }
  if (b < 0 || b >= m) {
    __int128 q = div_floor(b, m);
    __int128 r = b - q * m;
    ans += (unsigned long long)n * (unsigned long long)q;
    b = r;
  }
  __int128 y_max = a * n + b;
  if (y_max < m) return ans;
  return ans + ref_floor_sum(y_max / m, a, m, y_max % m);
}

void check_with_naive(long long n, long long m, long long a, long long b) {
  long long got = floor_sum(n, m, a, b);
  unsigned long long expected = naive_floor_sum(n, m, a, b);
  assert((unsigned long long)got == expected);
}

void check_with_ref(long long n, long long m, long long a, long long b) {
  long long got = floor_sum(n, m, a, b);
  unsigned long long expected = ref_floor_sum(n, m, a, b);
  assert((unsigned long long)got == expected);
}

constexpr bool test_constexpr() {
  if (floor_sum(0, 1, 0, 0) != 0) return false;
  if (floor_sum(0, LLONG_MAX, LLONG_MIN, LLONG_MAX) != 0) return false;
  if (floor_sum(4, 10, 6, 3) != 3) return false;
  if (floor_sum(6, 5, 4, 3) != 13) return false;
  if (floor_sum(4, 3, -2, 1) != -4) return false;
  if (floor_sum(1, 2, 0, -1) != -1) return false;
  if (floor_sum(1, 1, LLONG_MIN, LLONG_MIN) != LLONG_MIN) return false;
  if (floor_sum(1, 1, LLONG_MAX, LLONG_MAX) != LLONG_MAX) return false;
  // Overflow mod 2^64: sum_{i=0}^{2^32-1} i = 2^32 * (2^32 - 1) / 2
  // = 2^63 - 2^31 (fits in u64, overflows long long)
  if ((unsigned long long)floor_sum(1LL << 32, 1, 1, 0) !=
      ((1ULL << 63) - (1ULL << 31))) {
    return false;
  }
  // Negative overflow mod 2^64
  if ((unsigned long long)floor_sum(1LL << 32, 1, -1, 0) !=
      (0ULL - ((1ULL << 63) - (1ULL << 31)))) {
    return false;
  }
  return true;
}

void test_exhaustive_small() {
  for (long long n = 0; n <= 12; ++n) {
    for (long long m = 1; m <= 12; ++m) {
      for (long long a = -15; a <= 15; ++a) {
        for (long long b = -15; b <= 15; ++b) {
          check_with_naive(n, m, a, b);
        }
      }
    }
  }
}

void test_boundaries_and_overflow() {
  static_assert(test_constexpr());

  const std::vector<long long> vals_ab = {LLONG_MIN,
                                          LLONG_MIN + 1,
                                          LLONG_MIN + 2,
                                          -1000000000000LL,
                                          -(1LL << 32),
                                          -100,
                                          -2,
                                          -1,
                                          0,
                                          1,
                                          2,
                                          100,
                                          (1LL << 32),
                                          1000000000000LL,
                                          LLONG_MAX - 2,
                                          LLONG_MAX - 1,
                                          LLONG_MAX};
  const std::vector<long long> vals_m = {1,
                                         2,
                                         3,
                                         7,
                                         100,
                                         (1LL << 31) - 1,
                                         1LL << 31,
                                         1LL << 32,
                                         LLONG_MAX - 2,
                                         LLONG_MAX - 1,
                                         LLONG_MAX};
  const std::vector<long long> vals_n_small = {0, 1, 2, 3, 7, 100};
  const std::vector<long long> vals_n_large = {0,
                                               1,
                                               2,
                                               3,
                                               (1LL << 32) - 1,
                                               1LL << 32,
                                               (1LL << 32) + 1,
                                               LLONG_MAX - 2,
                                               LLONG_MAX - 1,
                                               LLONG_MAX};

  for (long long n : vals_n_small) {
    for (long long m : vals_m) {
      for (long long a : vals_ab) {
        for (long long b : vals_ab) {
          check_with_naive(n, m, a, b);
        }
      }
    }
  }

  for (long long n : vals_n_large) {
    for (long long m : vals_m) {
      for (long long a : vals_ab) {
        for (long long b : vals_ab) {
          check_with_ref(n, m, a, b);
        }
      }
    }
  }
}

void test_fibonacci_worst_cases() {
  std::vector<long long> fib = {1, 2};
  while (fib.back() <= LLONG_MAX - fib[fib.size() - 2]) {
    fib.push_back(fib.back() + fib[fib.size() - 2]);
  }
  for (int i = 1; i < (int)fib.size(); ++i) {
    long long m = fib[i];
    long long a = fib[i - 1];
    for (long long n : {0LL, 1LL, 2LL, 3LL, 10LL, 1000LL, m, LLONG_MAX}) {
      for (long long b :
           {0LL, 1LL, m - 1, -1LL, -m + 1, LLONG_MIN, LLONG_MAX}) {
        check_with_ref(n, m, a, b);
        check_with_ref(n, m, -a, b);
      }
    }
  }
}

void test_identities_and_random(std::mt19937_64& rng) {
  auto rand_range = [&](long long lo, long long hi) {
    return std::uniform_int_distribution<long long>(lo, hi)(rng);
  };

  // Small n against naive loop with huge m, a, b (verifies overflow mod 2^64)
  for (int iter = 0; iter < 5000; ++iter) {
    long long n = rand_range(0, 200);
    long long m = rand_range(1, LLONG_MAX);
    long long a = rand_range(LLONG_MIN, LLONG_MAX);
    long long b = rand_range(LLONG_MIN, LLONG_MAX);
    check_with_naive(n, m, a, b);
  }

  // Medium (32-bit range) and large (64-bit range) against reference
  for (int iter = 0; iter < 50000; ++iter) {
    long long n = rand_range(0, INT_MAX);
    long long m = rand_range(1, INT_MAX);
    long long a = rand_range(INT_MIN, INT_MAX);
    long long b = rand_range(INT_MIN, INT_MAX);
    check_with_ref(n, m, a, b);
  }
  for (int iter = 0; iter < 50000; ++iter) {
    long long n = rand_range(0, LLONG_MAX);
    long long m = rand_range(1, LLONG_MAX);
    long long a = rand_range(LLONG_MIN, LLONG_MAX);
    long long b = rand_range(LLONG_MIN, LLONG_MAX);
    check_with_ref(n, m, a, b);
  }

  // Algebraic identities on random large inputs
  for (int iter = 0; iter < 5000; ++iter) {
    long long n1 = rand_range(0, 1000000000LL);
    long long n2 = rand_range(0, 1000000000LL);
    long long m = rand_range(1, 1000000000LL);
    long long a = rand_range(-1000000000LL, 1000000000LL);
    long long b = rand_range(-1000000000LL, 1000000000LL);

    // Split: sum(n1 + n2) == sum(n1, b) + sum(n2, b + a * n1)
    unsigned long long s_all = (unsigned long long)floor_sum(n1 + n2, m, a, b);
    unsigned long long s_split =
        (unsigned long long)floor_sum(n1, m, a, b) +
        (unsigned long long)floor_sum(n2, m, a, b + a * n1);
    assert(s_all == s_split);

    // Negation: floor(-x / m) == -floor((x + m - 1) / m)
    unsigned long long s_neg =
        (unsigned long long)floor_sum(n1, m, -a, -b + m - 1);
    assert((unsigned long long)floor_sum(n1, m, a, b) + s_neg == 0ULL);

    // Reversal: sum_{i=0}^{n-1} f(i) == sum_{i=0}^{n-1} f(n - 1 - i)
    if (n1 > 0) {
      unsigned long long s_rev =
          (unsigned long long)floor_sum(n1, m, -a, b + a * (n1 - 1));
      assert((unsigned long long)floor_sum(n1, m, a, b) == s_rev);
    }

    // Scaling: floor_sum(n, c*m, c*a, c*b) == floor_sum(n, m, a, b)
    long long c = rand_range(1, 1000000000LL);
    assert(floor_sum(n1, c * m, c * a, c * b) == floor_sum(n1, m, a, b));
  }
}

}  // namespace

int main() {
  test_exhaustive_small();
  test_boundaries_and_overflow();
  test_fibonacci_worst_cases();

  std::mt19937_64 rng(0x9e3779b97f4a7c15ULL);
  test_identities_and_random(rng);
  return 0;
}
