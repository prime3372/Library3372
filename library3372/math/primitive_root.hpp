// Repository: Library3372
// File: library3372/math/primitive_root.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/math/primitive_root.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_MATH_PRIMITIVE_ROOT_HPP_
#define LIBRARY3372_LIBRARY3372_MATH_PRIMITIVE_ROOT_HPP_

#include <cassert>

#include "library3372/internal/math/montgomery.hpp"
#include "library3372/math/factorize.hpp"
#include "library3372/math/is_prime.hpp"

namespace library3372 {

// Smallest primitive root modulo a prime p
// https://en.wikipedia.org/wiki/Primitive_root_modulo_n
constexpr long long primitive_root(long long p) {
  assert(is_prime(p));
  if (p == 2) return 1;

  const unsigned long long u = (unsigned long long)p;
  const auto factors = factorize(p - 1);
  unsigned long long exps[16] = {};
  const int cnt = (int)factors.size();
  for (int i = 0; i < cnt; ++i) {
    exps[i] = (u - 1ULL) / (unsigned long long)factors[i].first;
  }

  const internal::Montgomery64 mont(u);
  const unsigned long long one = mont.one();
  unsigned long long gm = mont.add(one, one);
  for (long long g = 2;; ++g) {
    bool ok = true;
    for (int i = 0; i < cnt; ++i) {
      if (mont.pow(gm, exps[i]) == one) {
        ok = false;
        break;
      }
    }
    if (ok) return g;
    gm = mont.add(gm, one);
  }
}

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_MATH_PRIMITIVE_ROOT_HPP_
