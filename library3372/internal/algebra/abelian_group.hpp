// Repository: Library3372
// File: library3372/internal/algebra/abelian_group.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/internal/algebra/abelian_group.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_INTERNAL_ALGEBRA_ABELIAN_GROUP_HPP_
#define LIBRARY3372_LIBRARY3372_INTERNAL_ALGEBRA_ABELIAN_GROUP_HPP_

#include <concepts>
#include <type_traits>

#include "library3372/internal/type_traits/is_integer.hpp"
#include "library3372/internal/type_traits/to_unsigned.hpp"

namespace library3372::internal {

template <typename T>
concept AbelianGroup =
    std::is_object_v<T> && !std::is_const_v<T> && !std::is_volatile_v<T> &&
    !std::is_same_v<T, bool> && std::default_initializable<T> &&
    std::copyable<T> && requires(T& a, const T& b) {
      a += b;
      a -= b;
    };

template <typename T>
concept IntScalableAbelianGroup =
    AbelianGroup<T> && requires(T& a, int k) { a *= k; };

template <typename T>
  requires AbelianGroup<T>
struct AbelianGroupAdapter {
  using Storage = to_unsigned_t<T>;

  static void add_assign(Storage& a, const Storage& b) {
    if constexpr (sizeof(Storage) < sizeof(int) && is_integer_v<Storage>) {
      a = (Storage)(a + b);
    } else {
      a += b;
    }
  }

  static void sub_assign(Storage& a, const Storage& b) {
    if constexpr (sizeof(Storage) < sizeof(int) && is_integer_v<Storage>) {
      a = (Storage)(a - b);
    } else {
      a -= b;
    }
  }

  static Storage add_val(Storage a, const Storage& b) {
    add_assign(a, b);
    return a;
  }

  static Storage sub_val(Storage a, const Storage& b) {
    sub_assign(a, b);
    return a;
  }

  static Storage neg_val(const Storage& a) { return sub_val(Storage{}, a); }

  static Storage mul_int(int k, Storage x)
    requires IntScalableAbelianGroup<T>
  {
    if constexpr (is_integer_v<Storage>) {
      if constexpr (sizeof(Storage) < sizeof(int)) {
        return (Storage)((unsigned int)x * (unsigned int)k);
      } else {
        return x * (Storage)k;
      }
    } else if constexpr (std::is_floating_point_v<Storage> ||
                         std::is_same_v<Storage, __float128>) {
      x *= (Storage)k;
      return x;
    } else {
      x *= k;
      return x;
    }
  }
};

}  // namespace library3372::internal

#endif  // LIBRARY3372_LIBRARY3372_INTERNAL_ALGEBRA_ABELIAN_GROUP_HPP_
