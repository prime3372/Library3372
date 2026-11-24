// Repository: Library3372
// File: library3372/internal/math/modint_common.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/internal/math/modint_common.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_INTERNAL_MATH_MODINT_COMMON_HPP_
#define LIBRARY3372_LIBRARY3372_INTERNAL_MATH_MODINT_COMMON_HPP_

#include <cassert>
#include <charconv>
#include <istream>
#include <string>
#include <utility>

namespace library3372::internal {

template <typename Mint> constexpr Mint modint_pow(Mint x, long long n) {
  assert(n >= 0);
  Mint res = 1;
  while (n > 0) {
    if (n & 1) res *= x;
    x *= x;
    n >>= 1;
  }
  return res;
}

template <typename Mint> constexpr Mint modint_inv(int val, int m) {
  int s = m;
  int t = val;
  int m0 = 0;
  int m1 = 1;
  while (t != 0) {
    int u = s / t;
    s -= t * u;
    m0 -= m1 * u;
    std::swap(s, t);
    std::swap(m0, m1);
  }
  assert(s == 1);
  if (m0 < 0) m0 += m;
  return Mint::raw(m0);
}

template <typename Mint>
std::istream& modint_istream(std::istream& is, Mint& x) {
  std::string s;
  if (!(is >> s)) return is;
  const char* begin = s.data();
  const char* end = begin + s.size();
  if (s[0] == '-') {
    long long v = 0;
    [[maybe_unused]] std::from_chars_result res =
        std::from_chars(begin, end, v);
    assert(res.ec == std::errc() && res.ptr == end);
    x = v;
  } else {
    if (s[0] == '+') ++begin;
    unsigned long long v = 0;
    [[maybe_unused]] std::from_chars_result res =
        std::from_chars(begin, end, v);
    assert(res.ec == std::errc() && res.ptr == end);
    x = v;
  }
  return is;
}

}  // namespace library3372::internal

#endif  // LIBRARY3372_LIBRARY3372_INTERNAL_MATH_MODINT_COMMON_HPP_
