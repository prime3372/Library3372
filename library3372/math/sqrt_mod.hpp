// Repository: Library3372
// File: library3372/math/sqrt_mod.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/math/sqrt_mod.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_MATH_SQRT_MOD_HPP_
#define LIBRARY3372_LIBRARY3372_MATH_SQRT_MOD_HPP_

#include <bit>
#include <cassert>
#include <optional>

#include "library3372/internal/math/montgomery.hpp"
#include "library3372/math/is_prime.hpp"

namespace library3372 {
namespace internal_sqrt_mod {

constexpr int jacobi_bin(unsigned long long a, unsigned long long n) {
  int neg = 0;
  while (a != 0) {
    const int s = std::countr_zero(a);
    a >>= s;
    neg ^= (s & 1) & (int)((n ^ (n >> 1)) >> 1);
    if (a < n) {
      neg ^= (int)((a & n) >> 1) & 1;
      const unsigned long long tmp = a;
      a = n - a;
      n = tmp;
    } else {
      a -= n;
    }
  }
  return n == 1 ? (neg ? -1 : 1) : 0;
}

}  // namespace internal_sqrt_mod

// Hybrid Tonelli-Shanks / Atkin / Cipolla modular square root
// https://en.wikipedia.org/wiki/Tonelli%E2%80%93Shanks_algorithm
// https://en.wikipedia.org/wiki/Cipolla%27s_algorithm
constexpr std::optional<long long> sqrt_mod(long long a, long long p) {
  assert(is_prime(p));
  long long rem = a % p;
  if (rem < 0) rem += p;
  if (rem <= 1) return rem;

  const unsigned long long u = (unsigned long long)p;
  const internal::Montgomery64 mont(u);
  const unsigned long long one = mont.one();
  const unsigned long long am = mont.to_mont((unsigned long long)rem);

  const int s = std::countr_zero(u - 1ULL);
  if (s == 1) {
    const unsigned long long xm = mont.pow(am, (u + 1ULL) >> 2);
    if (mont.mul(xm, xm) != am) return std::nullopt;
    const long long res = (long long)mont.from_mont(xm);
    return res <= p - res ? res : p - res;
  }

  // Atkin's formula for p == 5 (mod 8)
  if (s == 2) {
    const unsigned long long two_a = mont.add(am, am);
    const unsigned long long v = mont.pow(two_a, (u - 5ULL) >> 3);
    const unsigned long long im = mont.mul(two_a, mont.mul(v, v));
    const unsigned long long xm = mont.mul(mont.mul(am, v), mont.sub(im, one));
    if (mont.mul(xm, xm) != am) return std::nullopt;
    const long long res = (long long)mont.from_mont(xm);
    return res <= p - res ? res : p - res;
  }

  const unsigned long long q = (u - 1ULL) >> s;
  const unsigned long long r = mont.pow(am, (q - 1ULL) >> 1);
  unsigned long long x = mont.mul(am, r);
  unsigned long long t = mont.mul(x, r);

  if (t != one) {
    int i = 0;
    unsigned long long tt = t;
    while (tt != one) {
      tt = mont.mul(tt, tt);
      ++i;
    }
    if (i == s) return std::nullopt;

    if (i * i >= 8 * std::bit_width(u)) {
      const unsigned long long ur = (unsigned long long)rem;
      unsigned long long t_int = 1ULL;
      unsigned long long w_int = 0;
      for (;;) {
        const unsigned long long t2 =
            (unsigned long long)(((unsigned __int128)t_int * t_int) % u);
        w_int = t2 >= ur ? t2 - ur : u - ur + t2;
        if (internal_sqrt_mod::jacobi_bin(w_int, u) == -1) break;
        ++t_int;
      }

      const unsigned long long tm = mont.to_mont(t_int);
      const unsigned long long wm = mont.to_mont(w_int);
      const unsigned long long e = (u + 1ULL) >> 1;
      unsigned long long rx = tm;
      unsigned long long ry = one;
      for (unsigned long long bit = 1ULL << (std::bit_width(e) - 2); bit > 0;
           bit >>= 1) {
        const unsigned long long xx = mont.mul(rx, rx);
        const unsigned long long yy = mont.mul(ry, ry);
        const unsigned long long xy = mont.mul(rx, ry);
        rx = mont.add(xx, mont.mul(yy, wm));
        ry = mont.add(xy, xy);
        if (e & bit) {
          const unsigned long long nx =
              mont.add(mont.mul(rx, tm), mont.mul(ry, wm));
          ry = mont.add(rx, mont.mul(ry, tm));
          rx = nx;
        }
      }

      const long long res = (long long)mont.from_mont(rx);
      return res <= p - res ? res : p - res;
    }

    unsigned long long z = 3;
    while (internal_sqrt_mod::jacobi_bin(z, u) != -1) ++z;
    unsigned long long c = mont.pow(mont.to_mont(z), q);

    int m = s;
    for (;;) {
      unsigned long long b = c;
      for (int j = 0; j < m - i - 1; ++j) b = mont.mul(b, b);
      m = i;
      c = mont.mul(b, b);
      x = mont.mul(x, b);
      t = mont.mul(t, c);
      if (t == one) break;
      i = 0;
      tt = t;
      while (tt != one) {
        tt = mont.mul(tt, tt);
        ++i;
      }
    }
  }

  const long long res = (long long)mont.from_mont(x);
  return res <= p - res ? res : p - res;
}

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_MATH_SQRT_MOD_HPP_
