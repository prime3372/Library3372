// Repository: Library3372
// File: library3372/math/convolution_large.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/math/convolution_large.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_MATH_CONVOLUTION_LARGE_HPP_
#define LIBRARY3372_LIBRARY3372_MATH_CONVOLUTION_LARGE_HPP_

#include <cassert>
#include <vector>

#include "library3372/internal/math/convolution_crt.hpp"
#include "library3372/internal/type_traits/is_integer.hpp"
#include "library3372/math/dynamic_modint.hpp"
#include "library3372/math/static_modint.hpp"

namespace library3372 {

template <int m>
std::vector<StaticModInt<m>> convolution_large(
    const std::vector<StaticModInt<m>>& a,
    const std::vector<StaticModInt<m>>& b) {
  return internal::convolution_arbitrary_mod32<25, 2113929217, 2013265921,
                                               1811939329, StaticModInt<m>>(
      a, b, m);
}

template <int id>
std::vector<DynamicModInt<id>> convolution_large(
    const std::vector<DynamicModInt<id>>& a,
    const std::vector<DynamicModInt<id>>& b) {
  return internal::convolution_arbitrary_mod32<25, 2113929217, 2013265921,
                                               1811939329, DynamicModInt<id>>(
      a, b, DynamicModInt<id>::mod());
}

template <typename T>
  requires internal::is_integer_v<T>
std::vector<T> convolution_large(const std::vector<T>& a,
                                 const std::vector<T>& b, long long mod) {
  assert(mod >= 1);
  if (mod <= 2147483647LL) {
    return internal::convolution_arbitrary_mod32<25, 2113929217, 2013265921,
                                                 1811939329, T>(a, b, (int)mod);
  }
  return internal::convolution_arbitrary_mod64<
      25, 2113929217, 2013265921, 1811939329, 1711276033, 1107296257, T>(a, b,
                                                                         mod);
}

template <long long m, typename T>
  requires internal::is_integer_v<T>
std::vector<T> convolution_large(const std::vector<T>& a,
                                 const std::vector<T>& b) {
  static_assert(m >= 1);
  if constexpr (m <= 2147483647LL) {
    return internal::convolution_arbitrary_mod32<25, 2113929217, 2013265921,
                                                 1811939329, T>(a, b, (int)m);
  } else {
    return internal::convolution_arbitrary_mod64<
        25, 2113929217, 2013265921, 1811939329, 1711276033, 1107296257, T>(a, b,
                                                                           m);
  }
}

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_MATH_CONVOLUTION_LARGE_HPP_
