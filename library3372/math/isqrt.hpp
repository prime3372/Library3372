// Repository: Library3372
// File: library3372/math/isqrt.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/math/isqrt.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_MATH_ISQRT_HPP_
#define LIBRARY3372_LIBRARY3372_MATH_ISQRT_HPP_

#include <bit>
#include <cassert>
#include <cmath>
#include <limits>
#include <type_traits>

#include "library3372/internal/type_traits/is_integer.hpp"

namespace library3372 {
namespace internal_isqrt {

constexpr unsigned int isqrt32(unsigned int u) {
  if (u <= 1) return u;
  if (!std::is_constant_evaluated()) {
    unsigned long long s = (unsigned long long)std::sqrt((double)u);
    while (s * s > u) --s;
    while ((s + 1) * (s + 1) <= u) ++s;
    return (unsigned int)s;
  }
  unsigned int x = 1U << ((std::bit_width(u) + 1) >> 1);
  while (true) {
    unsigned int nx = (x + u / x) >> 1;
    if (nx >= x) return x;
    x = nx;
  }
}

constexpr unsigned long long isqrt64(unsigned long long u) {
  if (u <= std::numeric_limits<unsigned int>::max()) {
    return isqrt32((unsigned int)u);
  }
  if (!std::is_constant_evaluated()) {
    unsigned long long s = (unsigned long long)std::sqrt((double)u);
    if (s > std::numeric_limits<unsigned int>::max()) {
      s = std::numeric_limits<unsigned int>::max();
    }
    while (s * s > u) --s;
    while (s < std::numeric_limits<unsigned int>::max() &&
           (s + 1) * (s + 1) <= u) {
      ++s;
    }
    return s;
  }
  unsigned long long x = 1ULL << ((std::bit_width(u) + 1) >> 1);
  while (true) {
    unsigned long long nx = (x + u / x) >> 1;
    if (nx >= x) return x;
    x = nx;
  }
}

constexpr unsigned __int128 isqrt128(unsigned __int128 u) {
  if (u <= std::numeric_limits<unsigned long long>::max()) {
    return isqrt64((unsigned long long)u);
  }
  unsigned __int128 x = 0;
  if (!std::is_constant_evaluated()) {
    const unsigned __int128 s0 = (unsigned __int128)std::sqrt((double)u);
    x = s0 + (s0 >> 50) + 2;
  } else {
    const unsigned long long hi = (unsigned long long)(u >> 64);
    const int w = 64 + std::bit_width(hi);
    x = (unsigned __int128)1 << ((w + 1) >> 1);
  }
  while (true) {
    const unsigned __int128 nx = (x + u / x) >> 1;
    if (nx >= x) return x;
    x = nx;
  }
}

}  // namespace internal_isqrt

// Integer square root via hardware sqrt + adjustment / Newton's method
// https://en.wikipedia.org/wiki/Integer_square_root
template <typename T>
  requires internal::is_integer_v<T>
constexpr T isqrt(T n) {
  if constexpr (std::is_same_v<std::remove_cv_t<T>, bool>) {
    return n;
  } else {
    if constexpr (internal::is_signed_integer_v<T>) assert(n >= 0);
    if (n <= 1) return n;
    if constexpr (sizeof(T) <= sizeof(unsigned int)) {
      return (T)internal_isqrt::isqrt32((unsigned int)n);
    } else if constexpr (sizeof(T) <= sizeof(unsigned long long)) {
      return (T)internal_isqrt::isqrt64((unsigned long long)n);
    } else {
      return (T)internal_isqrt::isqrt128((unsigned __int128)n);
    }
  }
}

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_MATH_ISQRT_HPP_
