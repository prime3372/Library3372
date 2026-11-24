// Repository: Library3372
// File: library3372/math/convolution_ll.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/math/convolution_ll.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_MATH_CONVOLUTION_LL_HPP_
#define LIBRARY3372_LIBRARY3372_MATH_CONVOLUTION_LL_HPP_

#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

#include "library3372/internal/math/convolution_garner.hpp"
#include "library3372/internal/math/ntt.hpp"

namespace library3372 {

// 64-bit integer convolution via 2-prime NTT and Chinese Remainder Theorem
// https://en.wikipedia.org/wiki/Chinese_remainder_theorem
inline std::vector<long long> convolution_ll(const std::vector<long long>& a,
                                             const std::vector<long long>& b) {
  if (a.empty() || b.empty()) return {};
  long long s = (long long)a.size() + (long long)b.size() - 1;
  assert(s <= (1LL << 24));

  int n = (int)a.size();
  int k = (int)b.size();
  if (std::min(n, k) <= internal::ntt_naive_threshold) {
    const long long* pa = a.data();
    const long long* pb = b.data();
    if (n < k) {
      std::swap(pa, pb);
      std::swap(n, k);
    }
    std::vector<long long> ans((int)s);
    for (int idx = 0; idx < (int)s; ++idx) {
      int lo = idx >= k - 1 ? idx - k + 1 : 0;
      int hi = idx < n ? idx : n - 1;
      unsigned long long sum = 0;
      for (int i = lo; i <= hi; ++i) {
        sum += (unsigned long long)pa[i] * (unsigned long long)pb[idx - i];
      }
      ans[idx] = (long long)sum;
    }
    return ans;
  }

  constexpr int m0 = 2130706433;
  constexpr int m1 = 2113929217;
  constexpr unsigned long long m01 = (unsigned long long)m0 * m1;
  constexpr unsigned long long m01_half = m01 >> 1;

  std::vector<long long> c((int)s);
  internal::convolution_garner<m0, m1>(
      [&](int i) { return a[i]; }, n, [&](int i) { return b[i]; }, k,
      (&a == &b),
      [&](int i, const unsigned int* x) {
        unsigned long long val = x[0] + (unsigned long long)m0 * x[1];
        if (val > m01_half) val -= m01;
        c[i] = (long long)val;
      });
  return c;
}

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_MATH_CONVOLUTION_LL_HPP_
