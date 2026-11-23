// Repository: Library3372
// File: library3372/math/convolution_arbitrary_mod.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/math/convolution_arbitrary_mod.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_MATH_CONVOLUTION_ARBITRARY_MOD_HPP_
#define LIBRARY3372_LIBRARY3372_MATH_CONVOLUTION_ARBITRARY_MOD_HPP_

#include <cassert>
#include <vector>

#include "library3372/internal/math/convolution_crt.hpp"
#include "library3372/internal/type_traits/is_integer.hpp"
#include "library3372/math/dynamic_modint.hpp"
#include "library3372/math/static_modint.hpp"

namespace library3372 {

template <int m>
std::vector<StaticModInt<m>> convolution_arbitrary_mod(
    const std::vector<StaticModInt<m>>& a,
    const std::vector<StaticModInt<m>>& b) {
  return internal::convolution_arbitrary_mod32<24, 167772161, 469762049,
                                               754974721, StaticModInt<m>>(a, b,
                                                                           m);
}

template <int id>
std::vector<DynamicModInt<id>> convolution_arbitrary_mod(
    const std::vector<DynamicModInt<id>>& a,
    const std::vector<DynamicModInt<id>>& b) {
  return internal::convolution_arbitrary_mod32<24, 167772161, 469762049,
                                               754974721, DynamicModInt<id>>(
      a, b, DynamicModInt<id>::mod());
}

template <typename T>
  requires internal::is_integer_v<T>
std::vector<T> convolution_arbitrary_mod(const std::vector<T>& a,
                                         const std::vector<T>& b, long long m) {
  assert(m >= 1);
  if (m <= 2147483647LL) {
    return internal::convolution_arbitrary_mod32<24, 167772161, 469762049,
                                                 754974721, T>(a, b, (int)m);
  }
  return internal::convolution_arbitrary_mod64<
      24, 2130706433, 2113929217, 2013265921, 1811939329, 1711276033, T>(a, b,
                                                                         m);
}

template <long long m, typename T>
  requires internal::is_integer_v<T>
std::vector<T> convolution_arbitrary_mod(const std::vector<T>& a,
                                         const std::vector<T>& b) {
  static_assert(m >= 1);
  if constexpr (m <= 2147483647LL) {
    return internal::convolution_arbitrary_mod32<24, 167772161, 469762049,
                                                 754974721, T>(a, b, (int)m);
  } else {
    return internal::convolution_arbitrary_mod64<
        24, 2130706433, 2113929217, 2013265921, 1811939329, 1711276033, T>(a, b,
                                                                           m);
  }
}

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_MATH_CONVOLUTION_ARBITRARY_MOD_HPP_
