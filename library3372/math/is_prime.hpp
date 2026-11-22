// Repository: Library3372
// File: library3372/math/is_prime.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/math/is_prime.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_MATH_IS_PRIME_HPP_
#define LIBRARY3372_LIBRARY3372_MATH_IS_PRIME_HPP_

#include <bit>

#include "library3372/internal/math/montgomery.hpp"

namespace library3372 {

// Deterministic Miller-Rabin primality test
// https://en.wikipedia.org/wiki/Miller%E2%80%93Rabin_primality_test
constexpr bool is_prime(long long n) {
  if (n <= 1) return false;
  if (n == 2 || n == 3 || n == 5 || n == 7 || n == 11 || n == 13) return true;
  const unsigned long long u = (unsigned long long)n;
  if ((u & 1ULL) == 0 || u % 3 == 0 || u % 5 == 0 || u % 7 == 0 ||
      u % 11 == 0 || u % 13 == 0) {
    return false;
  }

  unsigned long long d = u - 1;
  const int s = std::countr_zero(d);
  d >>= s;

  const internal::Montgomery64 mont(u);
  const unsigned long long one = mont.one();
  const unsigned long long neg_one = u - one;

  auto check_bases = [&](const unsigned long long* bases, int len) -> bool {
    for (int i = 0; i < len; ++i) {
      unsigned long long x = mont.to_mont(bases[i]);
      if (x == 0) continue;
      x = mont.pow(x, d);
      if (x == one || x == neg_one) continue;
      for (int r = 1; r < s; ++r) {
        x = mont.mul(x, x);
        if (x == neg_one) break;
      }
      if (x != neg_one) return false;
    }
    return true;
  };

  if (u < 4759123141ULL) {
    constexpr unsigned long long bases32[] = {2, 7, 61};
    return check_bases(bases32, 3);
  }
  constexpr unsigned long long bases64[] = {2,      325,     9375,      28178,
                                            450775, 9780504, 1795265022};
  return check_bases(bases64, 7);
}

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_MATH_IS_PRIME_HPP_
