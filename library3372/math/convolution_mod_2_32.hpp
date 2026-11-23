// Repository: Library3372
// File: library3372/math/convolution_mod_2_32.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/math/convolution_mod_2_32.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_MATH_CONVOLUTION_MOD_2_32_HPP_
#define LIBRARY3372_LIBRARY3372_MATH_CONVOLUTION_MOD_2_32_HPP_

#include <algorithm>
#include <cassert>
#include <type_traits>
#include <utility>
#include <vector>

#include "library3372/internal/math/convolution_garner.hpp"
#include "library3372/internal/math/ntt.hpp"

namespace library3372 {

// Convolution modulo 2^32 via 3-prime NTT and Garner's CRT reconstruction
// https://en.wikipedia.org/wiki/Chinese_remainder_theorem
template <typename T>
  requires(std::is_same_v<T, int> || std::is_same_v<T, unsigned int>)
std::vector<T> convolution_mod_2_32(const std::vector<T>& a,
                                    const std::vector<T>& b) {
  if (a.empty() || b.empty()) return {};
  int n = (int)a.size();
  int k = (int)b.size();
  long long s = (long long)n + k - 1;
  assert(s <= (1LL << 24));

  if (std::min(n, k) <= internal::ntt_naive_threshold) {
    const T* pa = a.data();
    const T* pb = b.data();
    if (n < k) {
      std::swap(pa, pb);
      std::swap(n, k);
    }
    std::vector<T> ans((int)s);
    for (int idx = 0; idx < (int)s; ++idx) {
      int lo = idx >= k - 1 ? idx - k + 1 : 0;
      int hi = idx < n ? idx : n - 1;
      unsigned int sum = 0;
      for (int i = lo; i <= hi; ++i) {
        sum += (unsigned int)pa[i] * (unsigned int)pb[idx - i];
      }
      ans[idx] = (T)sum;
    }
    return ans;
  }

  constexpr int m0 = 2130706433;
  constexpr int m1 = 2113929217;
  constexpr int m2 = 2013265921;
  constexpr unsigned int m0_u32 = (unsigned int)m0;
  constexpr unsigned int m01_u32 = (unsigned int)((unsigned long long)m0 * m1);

  std::vector<T> res((int)s);
  internal::convolution_garner<m0, m1, m2>(
      [&](int i) { return (unsigned int)a[i]; }, n,
      [&](int i) { return (unsigned int)b[i]; }, k, (&a == &b),
      [&](int i, const unsigned int* x) {
        res[i] = (T)(unsigned int)(x[0] + m0_u32 * x[1] + m01_u32 * x[2]);
      });
  return res;
}

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_MATH_CONVOLUTION_MOD_2_32_HPP_
