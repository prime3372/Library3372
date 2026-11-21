// Repository: Library3372
// File: library3372/internal/type_traits/is_integer.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/internal/type_traits/is_integer.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_INTERNAL_TYPE_TRAITS_IS_INTEGER_HPP_
#define LIBRARY3372_LIBRARY3372_INTERNAL_TYPE_TRAITS_IS_INTEGER_HPP_

#include <type_traits>

namespace library3372::internal {

template <typename T>
struct is_signed_integer
    : std::bool_constant<(std::is_integral_v<T> && std::is_signed_v<T>) ||
                         std::is_same_v<std::remove_cv_t<T>, __int128>> {};

template <typename T>
inline constexpr bool is_signed_integer_v = is_signed_integer<T>::value;

template <typename T>
struct is_unsigned_integer
    : std::bool_constant<
          (std::is_integral_v<T> && std::is_unsigned_v<T>) ||
          std::is_same_v<std::remove_cv_t<T>, unsigned __int128>> {};

template <typename T>
inline constexpr bool is_unsigned_integer_v = is_unsigned_integer<T>::value;

template <typename T>
struct is_integer
    : std::bool_constant<
          std::is_integral_v<T> ||
          std::is_same_v<std::remove_cv_t<T>, __int128> ||
          std::is_same_v<std::remove_cv_t<T>, unsigned __int128>> {};

template <typename T> inline constexpr bool is_integer_v = is_integer<T>::value;

}  // namespace library3372::internal

#endif  // LIBRARY3372_LIBRARY3372_INTERNAL_TYPE_TRAITS_IS_INTEGER_HPP_
