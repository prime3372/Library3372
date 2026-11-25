// Repository: Library3372
// File: library3372/internal/type_traits/to_unsigned.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/internal/type_traits/to_unsigned.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_INTERNAL_TYPE_TRAITS_TO_UNSIGNED_HPP_
#define LIBRARY3372_LIBRARY3372_INTERNAL_TYPE_TRAITS_TO_UNSIGNED_HPP_

#include <type_traits>

#include "library3372/internal/type_traits/is_integer.hpp"

namespace library3372::internal {

template <typename T> struct to_unsigned {
  using type = T;
};

template <typename T>
  requires(is_signed_integer_v<T> &&
           !std::is_same_v<std::remove_cv_t<T>, __int128>)
struct to_unsigned<T> {
  using type = std::make_unsigned_t<T>;
};

template <> struct to_unsigned<__int128> {
  using type = unsigned __int128;
};

template <> struct to_unsigned<const __int128> {
  using type = const unsigned __int128;
};

template <> struct to_unsigned<volatile __int128> {
  using type = volatile unsigned __int128;
};

template <> struct to_unsigned<const volatile __int128> {
  using type = const volatile unsigned __int128;
};

template <typename T> using to_unsigned_t = typename to_unsigned<T>::type;

}  // namespace library3372::internal

#endif  // LIBRARY3372_LIBRARY3372_INTERNAL_TYPE_TRAITS_TO_UNSIGNED_HPP_
