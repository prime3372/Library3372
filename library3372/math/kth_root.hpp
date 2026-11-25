// Repository: Library3372
// File: library3372/math/kth_root.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/math/kth_root.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_MATH_KTH_ROOT_HPP_
#define LIBRARY3372_LIBRARY3372_MATH_KTH_ROOT_HPP_

#include <bit>
#include <cassert>
#include <cmath>
#include <type_traits>

#include "library3372/internal/type_traits/is_integer.hpp"
#include "library3372/math/isqrt.hpp"

namespace library3372 {
namespace internal_kth_root {

template <typename U> constexpr int bit_width_u(U u) {
  if constexpr (sizeof(U) <= sizeof(unsigned long long)) {
    return std::bit_width(u);
  } else {
    const unsigned long long hi = (unsigned long long)(u >> 64);
    return hi ? 64 + std::bit_width(hi) : std::bit_width((unsigned long long)u);
  }
}

template <typename U> constexpr bool mul_le(U a, U b, U u, U& out) {
  if constexpr (sizeof(U) <= sizeof(unsigned int)) {
    const unsigned long long p = (unsigned long long)a * b;
    if (p > u) return false;
    out = (U)p;
    return true;
  } else if constexpr (sizeof(U) <= sizeof(unsigned long long)) {
    const unsigned __int128 p = (unsigned __int128)a * b;
    if (p > u) return false;
    out = (U)p;
    return true;
  } else {
    return !__builtin_mul_overflow(a, b, &out) && out <= u;
  }
}

template <typename U> constexpr bool pow_le(U x, unsigned int k, U u) {
  U res = 1;
  while (true) {
    if ((k & 1U) && !mul_le(res, x, u, res)) return false;
    k >>= 1;
    if (k == 0) return true;
    if (!mul_le(x, x, u, x)) return false;
  }
}

template <typename U> constexpr bool cube_le(unsigned long long s, U u) {
  if constexpr (sizeof(U) <= sizeof(unsigned int)) {
    return (unsigned long long)s * s * s <= u;
  } else if constexpr (sizeof(U) <= sizeof(unsigned long long)) {
    return (unsigned __int128)s * s * s <= u;
  } else {
    const unsigned __int128 sq = (unsigned __int128)s * s;
    unsigned __int128 cube = 0;
    return !__builtin_mul_overflow(sq, (unsigned __int128)s, &cube) &&
           cube <= u;
  }
}

template <typename U, typename K>
constexpr unsigned long long kth_root_u(U u, K k) {
  const int w = bit_width_u(u);
  using UK = std::conditional_t<(sizeof(K) > sizeof(unsigned long long)),
                                unsigned __int128, unsigned long long>;
  if ((UK)k >= (UK)w) return 1;
  const unsigned int uk = (unsigned int)k;
  const int b = (w - 1) / (int)uk;
  const unsigned long long lo_bound = 1ULL << b;
  const unsigned long long hi_bound = lo_bound << 1;

  if (std::is_constant_evaluated() || b <= 3) {
    unsigned long long lo = lo_bound;
    unsigned long long hi = hi_bound;
    while (hi - lo > 1) {
      const unsigned long long mid = (lo + hi) >> 1;
      if (pow_le((U)mid, uk, u)) {
        lo = mid;
      } else {
        hi = mid;
      }
    }
    return lo;
  }

  auto clamp_guess = [&](unsigned long long s) -> unsigned long long {
    if (s < lo_bound) return lo_bound;
    if (s >= hi_bound) return hi_bound - 1;
    return s;
  };

  if (uk == 3) {
    unsigned long long s =
        clamp_guess((unsigned long long)std::cbrt((double)u));
    if (!cube_le(s, u)) {
      do {
        --s;
      } while (!cube_le(s, u));
    } else {
      while (cube_le(s + 1, u)) ++s;
    }
    return s;
  }

  unsigned long long s = clamp_guess(
      (unsigned long long)std::exp2(std::log2((double)u) / (double)uk));
  if (!pow_le((U)s, uk, u)) {
    do {
      --s;
    } while (!pow_le((U)s, uk, u));
  } else {
    while (pow_le((U)(s + 1), uk, u)) ++s;
  }
  return s;
}

}  // namespace internal_kth_root

// Integer k-th root via floating-point approximation + adjustment / binary search
// https://en.wikipedia.org/wiki/Nth_root
template <typename T, typename K>
  requires(internal::is_integer_v<T> && internal::is_integer_v<K>)
constexpr T kth_root(T n, K k) {
  if constexpr (internal::is_signed_integer_v<T>) assert(n >= 0);
  if constexpr (std::is_same_v<std::remove_cv_t<K>, bool>) {
    assert(k);
    return n;
  } else {
    assert(k >= (K)1);
    if constexpr (std::is_same_v<std::remove_cv_t<T>, bool>) {
      return n;
    } else {
      if (n <= 1 || k == (K)1) return n;
      if (k == (K)2) return isqrt(n);
      if (k == (K)4) return isqrt(isqrt(n));
      if constexpr (sizeof(T) <= sizeof(unsigned int)) {
        return (T)internal_kth_root::kth_root_u((unsigned int)n, k);
      } else if constexpr (sizeof(T) <= sizeof(unsigned long long)) {
        return (T)internal_kth_root::kth_root_u((unsigned long long)n, k);
      } else {
        return (T)internal_kth_root::kth_root_u((unsigned __int128)n, k);
      }
    }
  }
}

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_MATH_KTH_ROOT_HPP_
