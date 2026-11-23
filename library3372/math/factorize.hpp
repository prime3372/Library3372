// Repository: Library3372
// File: library3372/math/factorize.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/math/factorize.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_MATH_FACTORIZE_HPP_
#define LIBRARY3372_LIBRARY3372_MATH_FACTORIZE_HPP_

#include <algorithm>
#include <bit>
#include <cassert>
#include <numeric>
#include <utility>
#include <vector>

#include "library3372/math/is_prime.hpp"

namespace library3372 {
namespace internal_factorize {

// 64-bit Montgomery arithmetic without R^2 precomputation for Pollard's rho
// https://en.wikipedia.org/wiki/Montgomery_modular_multiplication
struct Mont64Rho {
  unsigned long long mod;
  unsigned long long mod2;
  unsigned long long inv;
  unsigned long long neg_inv;
  unsigned long long one;

  explicit constexpr Mont64Rho(unsigned long long m)
      : mod(m), mod2(m * 2ULL), inv(m), neg_inv(0), one((-m) % m) {
    assert((m & 1ULL) == 1ULL);
    for (int i = 0; i < 5; ++i) inv *= 2ULL - m * inv;
    neg_inv = 0ULL - inv;
  }

  constexpr unsigned long long add(unsigned long long a,
                                   unsigned long long b) const {
    return b >= mod - a ? a - (mod - b) : a + b;
  }

  constexpr unsigned long long mul(unsigned long long a,
                                   unsigned long long b) const {
    unsigned __int128 t = (unsigned __int128)a * b;
    unsigned long long m = (unsigned long long)t * inv;
    unsigned long long high_t = (unsigned long long)(t >> 64);
    unsigned long long high_mn =
        (unsigned long long)(((unsigned __int128)m * mod) >> 64);
    return high_t < high_mn ? high_t - high_mn + mod : high_t - high_mn;
  }

  constexpr unsigned long long mul_2m(unsigned long long a,
                                      unsigned long long b) const {
    unsigned __int128 t = (unsigned __int128)a * b;
    unsigned long long q = (unsigned long long)t * neg_inv;
    return (unsigned long long)((t + (unsigned __int128)q * mod) >> 64);
  }
};

// Multi-walk Brent's cycle detection variant of Pollard's rho algorithm
// https://en.wikipedia.org/wiki/Pollard%27s_rho_algorithm
template <bool Lazy, int Walks, int Lim>
constexpr unsigned long long pollard_rho_impl(unsigned long long n) {
  const Mont64Rho mont(n);
  auto for_walks = [&](auto&& fn) constexpr {
    [&]<int... Ws>(std::integer_sequence<int, Ws...>) {
      (fn(std::integral_constant<int, Ws>{}), ...);
    }(std::make_integer_sequence<int, Walks>{});
  };

  unsigned long long c[Walks] = {};
  c[0] = mont.one;
  for (int w = 1; w < Walks; ++w) c[w] = mont.add(c[w - 1], mont.one);

  for (;;) {
    unsigned long long y[Walks] = {};
    unsigned long long q[Walks] = {};
    y[0] = mont.add(mont.one, mont.one);
    q[0] = mont.one;
    for (int w = 1; w < Walks; ++w) {
      y[w] = mont.add(y[w - 1], mont.one);
      q[w] = mont.one;
    }
    unsigned long long g = 1;
    unsigned long long r = 1;
    while (g == 1) {
      unsigned long long x[Walks] = {};
      for_walks([&](auto w) { x[w] = y[w]; });
      unsigned long long k = 0;
      while (k < r && g == 1) {
        unsigned long long ys[Walks] = {};
        for_walks([&](auto w) { ys[w] = y[w]; });
        const unsigned long long lim =
            (r - k < (unsigned long long)Lim ? r - k : (unsigned long long)Lim);
        for (unsigned long long i = 0; i < lim; ++i) {
          for_walks([&](auto w) {
            if constexpr (Lazy) {
              y[w] = mont.mul_2m(y[w], y[w]) + c[w];
              if (y[w] >= mont.mod2) y[w] -= mont.mod2;
              const unsigned long long diff =
                  x[w] > y[w] ? x[w] - y[w] : y[w] - x[w];
              q[w] = mont.mul_2m(q[w], diff);
            } else {
              y[w] = mont.add(mont.mul(y[w], y[w]), c[w]);
              const unsigned long long diff =
                  x[w] > y[w] ? x[w] - y[w] : y[w] - x[w];
              q[w] = mont.mul(q[w], diff);
            }
          });
        }
        unsigned long long prod = q[0];
        for (int w = 1; w < Walks; ++w) {
          prod = Lazy ? mont.mul_2m(prod, q[w]) : mont.mul(prod, q[w]);
        }
        g = std::gcd(prod, n);
        k += lim;
        if (g == n) {
          for_walks([&](auto w) { y[w] = ys[w]; });
          for (unsigned long long i = 0; i < lim; ++i) {
            for (int w = 0; w < Walks; ++w) {
              if constexpr (Lazy) {
                y[w] = mont.mul_2m(y[w], y[w]) + c[w];
                if (y[w] >= mont.mod2) y[w] -= mont.mod2;
              } else {
                y[w] = mont.add(mont.mul(y[w], y[w]), c[w]);
              }
              const unsigned long long diff =
                  x[w] > y[w] ? x[w] - y[w] : y[w] - x[w];
              g = std::gcd(diff, n);
              if (g != 1) {
                if (g < n) return g;
                break;
              }
            }
            if (g != 1) break;
          }
          break;
        }
      }
      r <<= 1;
    }
    if (g > 1 && g < n) return g;
    c[0] = mont.add(c[Walks - 1], mont.one);
    for (int w = 1; w < Walks; ++w) c[w] = mont.add(c[w - 1], mont.one);
  }
}

constexpr unsigned long long pollard_rho(unsigned long long n) {
  if ((n & 1ULL) == 0) return 2;
  if (n < (1ULL << 32)) return pollard_rho_impl<true, 1, 64>(n);
  if (n < (1ULL << 62)) return pollard_rho_impl<true, 3, 512>(n);
  return pollard_rho_impl<false, 3, 512>(n);
}

constexpr void factor_odd(unsigned long long n, bool known_composite,
                          unsigned long long* factors, int& cnt) {
  if (n <= 1) return;
  if (!known_composite && (n < 9ULL || is_prime((long long)n))) {
    factors[cnt++] = n;
    return;
  }
  unsigned long long d = pollard_rho(n);
  factor_odd(d, false, factors, cnt);
  factor_odd(n / d, false, factors, cnt);
}

}  // namespace internal_factorize

// Prime factorization via Pollard's rho algorithm
// https://en.wikipedia.org/wiki/Pollard%27s_rho_algorithm
constexpr std::vector<std::pair<long long, int>> factorize(long long n) {
  assert(n >= 1);
  if (n == 1) return {};

  std::vector<std::pair<long long, int>> res;
  unsigned long long u = (unsigned long long)n;
  int tz = std::countr_zero(u);
  if (tz > 0) {
    res.emplace_back(2LL, tz);
    u >>= tz;
  }
  if (u == 1) return res;

  if (u < (1ULL << 15)) {
    for (unsigned long long d = 3; d * d <= u; d += 2) {
      if (u % d == 0) {
        int e = 0;
        do {
          u /= d;
          ++e;
        } while (u % d == 0);
        res.emplace_back((long long)d, e);
      }
    }
    if (u > 1) res.emplace_back((long long)u, 1);
    return res;
  }

  if (is_prime((long long)u)) {
    res.emplace_back((long long)u, 1);
    return res;
  }

  unsigned long long factors[64] = {};
  int cnt = 0;
  internal_factorize::factor_odd(u, true, factors, cnt);
  std::sort(factors, factors + cnt);
  for (int i = 0; i < cnt;) {
    int j = i + 1;
    while (j < cnt && factors[j] == factors[i]) ++j;
    res.emplace_back((long long)factors[i], j - i);
    i = j;
  }
  return res;
}

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_MATH_FACTORIZE_HPP_
