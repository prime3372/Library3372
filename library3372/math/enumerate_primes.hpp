// Repository: Library3372
// File: library3372/math/enumerate_primes.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/math/enumerate_primes.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_MATH_ENUMERATE_PRIMES_HPP_
#define LIBRARY3372_LIBRARY3372_MATH_ENUMERATE_PRIMES_HPP_

#include <cassert>
#include <vector>

#include "library3372/internal/math/prime_sieve.hpp"

namespace library3372 {

constexpr std::vector<int> enumerate_primes(int n) {
  assert(n >= 0);
  if (n < 2) return {};
  std::vector<int> res;
  res.reserve(internal::estimate_prime_count((unsigned long long)n,
                                             (unsigned long long)n) +
              3U);
  res.push_back(2);
  if (n >= 3) res.push_back(3);
  if (n >= 5) res.push_back(5);
  if (n >= 7) {
    const unsigned long long ur = (unsigned long long)n + 1ULL;
    internal::sieve_primes_wheel30(
        7ULL, ur, internal::isqrt_u64((unsigned long long)n),
        [&](unsigned long long p) { res.push_back((int)p); });
  }
  return res;
}

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_MATH_ENUMERATE_PRIMES_HPP_
