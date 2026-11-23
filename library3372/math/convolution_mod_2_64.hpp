// Repository: Library3372
// File: library3372/math/convolution_mod_2_64.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/math/convolution_mod_2_64.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_MATH_CONVOLUTION_MOD_2_64_HPP_
#define LIBRARY3372_LIBRARY3372_MATH_CONVOLUTION_MOD_2_64_HPP_

#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

#include "library3372/internal/math/convolution_crt.hpp"
#include "library3372/internal/math/ntt.hpp"
#include "library3372/internal/type_traits/is_integer.hpp"

namespace library3372 {

// Convolution modulo 2^64 via 5-prime NTT and Garner's CRT reconstruction
// https://en.wikipedia.org/wiki/Chinese_remainder_theorem
template <typename T = unsigned long long>
  requires internal::is_integer_v<T>
std::vector<T> convolution_mod_2_64(const std::vector<T>& a,
                                    const std::vector<T>& b) {
  if (a.empty() || b.empty()) return {};
  int n = (int)a.size();
  int k = (int)b.size();
  long long s = (long long)n + k - 1;
  assert(s <= (1LL << 24));

  if (std::min(n, k) <= internal::ntt_naive_threshold) {
    const std::vector<T>* pa = &a;
    const std::vector<T>* pb = &b;
    if (n < k) {
      std::swap(pa, pb);
      std::swap(n, k);
    }
    std::vector<T> ans((int)s);
    for (int idx = 0; idx < (int)s; ++idx) {
      int lo = idx >= k - 1 ? idx - k + 1 : 0;
      int hi = idx < n ? idx : n - 1;
      unsigned long long sum = 0;
      for (int i = lo; i <= hi; ++i) {
        sum +=
            (unsigned long long)(*pa)[i] * (unsigned long long)(*pb)[idx - i];
      }
      ans[idx] = (T)sum;
    }
    return ans;
  }

  constexpr int m0 = 2130706433;
  constexpr int m1 = 2113929217;
  constexpr int m2 = 2013265921;
  constexpr int m3 = 1811939329;
  constexpr int m4 = 1711276033;
  constexpr unsigned long long m0_u64 = (unsigned long long)m0;
  constexpr unsigned long long m01_u64 = m0_u64 * m1;
  constexpr unsigned long long m012_u64 = m01_u64 * m2;
  constexpr unsigned long long m0123_u64 = m012_u64 * m3;

  std::vector<T> res((int)s);
  internal::convolution_crt<m0, m1, m2, m3, m4>(
      [&](int i) { return (unsigned long long)a[i]; }, n,
      [&](int i) { return (unsigned long long)b[i]; }, k, (&a == &b),
      [&](int i, const unsigned int* x) {
        unsigned long long val = x[0] + m0_u64 * x[1] + m01_u64 * x[2] +
                                 m012_u64 * x[3] + m0123_u64 * x[4];
        res[i] = (T)val;
      });
  return res;
}

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_MATH_CONVOLUTION_MOD_2_64_HPP_
