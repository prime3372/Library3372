// Repository: Library3372
// File: library3372/math/floor_sum.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/math/floor_sum.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_MATH_FLOOR_SUM_HPP_
#define LIBRARY3372_LIBRARY3372_MATH_FLOOR_SUM_HPP_

#include <cassert>
#include <utility>

namespace library3372 {
namespace internal_floor_sum {

constexpr unsigned long long tri(unsigned long long n) {
  return (n >> 1) * ((n - 1ULL) | (n & 1ULL));
}

constexpr void normalize_neg(long long& x, long long m,
                             unsigned long long weight,
                             unsigned long long& ans) {
  long long q = x / m;
  long long r = x % m;
  if (r < 0) {
    r += m;
    --q;
  }
  ans += weight * (unsigned long long)q;
  x = r;
}

template <typename U>
constexpr void step_reduce(unsigned long long& ans, U n, U& m, U& a, U& b) {
  std::swap(m, a);
  if ((a >> 1) < m) {
    ans += tri(n);
    a -= m;
    if (b >= m) {
      ans += n;
      b -= m;
    }
  } else {
    ans += tri(n) * (a / m);
    a %= m;
    if (b >= m) {
      ans += (unsigned long long)n * (b / m);
      b %= m;
    }
  }
}

constexpr unsigned long long floor_sum_u32(unsigned long long ans,
                                           unsigned int n, unsigned int m,
                                           unsigned int a, unsigned int b) {
  while (true) {
    const unsigned long long y_max = (unsigned long long)a * n + b;
    if (y_max < m) return ans;
    n = (unsigned int)(y_max / m);
    b = (unsigned int)(y_max % m);
    step_reduce(ans, n, m, a, b);
  }
}

}  // namespace internal_floor_sum

// Sum of floor of linear function via Euclidean lattice-point reduction
// https://atcoder.github.io/ac-library/production/document_en/math.html
constexpr long long floor_sum(long long n, long long m, long long a,
                              long long b) {
  assert(n >= 0);
  assert(m >= 1);
  if (n == 0) return 0;

  unsigned long long un = (unsigned long long)n;
  unsigned long long um = (unsigned long long)m;
  unsigned long long ans = 0;

  if (a < 0) {
    internal_floor_sum::normalize_neg(a, m, internal_floor_sum::tri(un), ans);
  }
  if (b < 0) {
    internal_floor_sum::normalize_neg(b, m, un, ans);
  }

  unsigned long long ua = (unsigned long long)a;
  unsigned long long ub = (unsigned long long)b;

  if (ua >= um) {
    ans += internal_floor_sum::tri(un) * (ua / um);
    ua %= um;
  }
  if (ub >= um) {
    ans += un * (ub / um);
    ub %= um;
  }

  while (true) {
    if ((un | um) <= 0xffffffffULL) {
      return (long long)internal_floor_sum::floor_sum_u32(
          ans, (unsigned int)un, (unsigned int)um, (unsigned int)ua,
          (unsigned int)ub);
    }
    const unsigned __int128 y_max = (unsigned __int128)ua * un + ub;
    if (y_max < um) break;
    if ((y_max >> 64) == 0) {
      const unsigned long long y64 = (unsigned long long)y_max;
      un = y64 / um;
      ub = y64 % um;
    } else {
      un = (unsigned long long)(y_max / um);
      ub = (unsigned long long)(y_max % um);
    }
    internal_floor_sum::step_reduce(ans, un, um, ua, ub);
  }
  return (long long)ans;
}

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_MATH_FLOOR_SUM_HPP_
