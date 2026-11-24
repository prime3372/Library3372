// Repository: Library3372
// File: library3372/math/enumerate_quotients.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/math/enumerate_quotients.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_MATH_ENUMERATE_QUOTIENTS_HPP_
#define LIBRARY3372_LIBRARY3372_MATH_ENUMERATE_QUOTIENTS_HPP_

#include <cassert>
#include <limits>
#include <tuple>
#include <vector>

#include "library3372/math/isqrt.hpp"

namespace library3372 {
namespace internal_enumerate_quotients {

template <typename U>
constexpr void enumerate_quotients_impl(
    U n, U s, U k,
    std::vector<std::tuple<long long, long long, long long>>& res) {
  long long r = (long long)n + 1;
  for (U q = 1; q <= s; ++q) {
    const long long l = (long long)(n / (q + 1)) + 1;
    res.emplace_back((long long)q, l, r);
    r = l;
  }
  for (U x = k; x >= 1; --x) {
    res.emplace_back((long long)(n / x), (long long)x, (long long)x + 1);
  }
}

}  // namespace internal_enumerate_quotients

// Quotient enumeration via Dirichlet hyperbola split
// https://en.wikipedia.org/wiki/Dirichlet_hyperbola_method
constexpr std::vector<std::tuple<long long, long long, long long>>
enumerate_quotients(long long n) {
  assert(0 <= n && n < std::numeric_limits<long long>::max());
  if (n == 0) return {};

  using Result = std::vector<std::tuple<long long, long long, long long>>;
  Result res;
  const unsigned long long u = (unsigned long long)n;
  const unsigned long long s = isqrt(u);
  const unsigned long long k = u / (s + 1);
  res.reserve((typename Result::size_type)(s + k));

  if (u <= std::numeric_limits<unsigned int>::max()) {
    internal_enumerate_quotients::enumerate_quotients_impl(
        (unsigned int)u, (unsigned int)s, (unsigned int)k, res);
  } else {
    internal_enumerate_quotients::enumerate_quotients_impl(u, s, k, res);
  }
  return res;
}

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_MATH_ENUMERATE_QUOTIENTS_HPP_
