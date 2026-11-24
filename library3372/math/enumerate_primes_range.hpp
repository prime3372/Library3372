// Repository: Library3372
// File: library3372/math/enumerate_primes_range.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/math/enumerate_primes_range.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_MATH_ENUMERATE_PRIMES_RANGE_HPP_
#define LIBRARY3372_LIBRARY3372_MATH_ENUMERATE_PRIMES_RANGE_HPP_

#include <cassert>
#include <vector>

#include "library3372/internal/math/prime_sieve.hpp"
#include "library3372/math/is_prime.hpp"
#include "library3372/math/isqrt.hpp"

namespace library3372 {

constexpr std::vector<long long> enumerate_primes_range(long long l,
                                                        long long r) {
  assert(0 <= l && l <= r);
  if (r <= 2 || l == r) return {};
  std::vector<long long> res;
  res.reserve(internal::estimate_prime_count((unsigned long long)(r - l),
                                             (unsigned long long)(r - 1)) +
              3U);
  if (l <= 2 && 2 < r) res.push_back(2);
  if (l <= 3 && 3 < r) res.push_back(3);
  if (l <= 5 && 5 < r) res.push_back(5);

  const unsigned long long ul =
      (unsigned long long)l < 7ULL ? 7ULL : (unsigned long long)l;
  const unsigned long long ur = (unsigned long long)r;
  if (ul < ur) {
    const unsigned int limit = (unsigned int)isqrt(ur - 1ULL);
    const bool use_is_prime =
        limit > 1000U && (unsigned long long)limit > (ur - ul) * 32ULL;
    if (use_is_prime) {
      internal::sieve_primes_wheel30(ul, ur, 1000U, [&](unsigned long long p) {
        if (is_prime((long long)p)) res.push_back((long long)p);
      });
    } else {
      internal::sieve_primes_wheel30(ul, ur, limit, [&](unsigned long long p) {
        res.push_back((long long)p);
      });
    }
  }
  return res;
}

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_MATH_ENUMERATE_PRIMES_RANGE_HPP_
