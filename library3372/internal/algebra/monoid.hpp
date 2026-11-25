// Repository: Library3372
// File: library3372/internal/algebra/monoid.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/internal/algebra/monoid.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_INTERNAL_ALGEBRA_MONOID_HPP_
#define LIBRARY3372_LIBRARY3372_INTERNAL_ALGEBRA_MONOID_HPP_

#include <concepts>
#include <type_traits>
#include <utility>

namespace library3372::internal {

template <auto fn> consteval bool is_non_null_callable() {
  if constexpr (std::is_pointer_v<decltype(fn)>) return fn != nullptr;
  return true;
}

template <typename S, auto op, auto e>
concept Monoid =
    std::is_object_v<S> && !std::is_const_v<S> && !std::is_volatile_v<S> &&
    std::copyable<S> && is_non_null_callable<op>() &&
    is_non_null_callable<e>() && requires(const S& a, const S& b) {
      { op(a, b) } -> std::convertible_to<S>;
      { e() } -> std::convertible_to<S>;
    };

template <typename S, auto op, auto e>
  requires Monoid<S, op, e>
struct MonoidAdapter {
  using Storage = std::conditional_t<std::is_same_v<S, bool>, unsigned char, S>;

  static decltype(auto) to_s(const Storage& x) {
    if constexpr (std::is_same_v<S, bool>) {
      return (bool)x;
    } else {
      return x;
    }
  }

  template <typename U> static decltype(auto) to_storage(U&& x) {
    if constexpr (std::is_same_v<std::remove_cvref_t<U>, Storage>) {
      return std::forward<U>(x);
    } else {
      return (Storage)std::forward<U>(x);
    }
  }

  static S call_e() {
    if constexpr (std::is_same_v<std::remove_cvref_t<decltype(e())>, S>) {
      return e();
    } else {
      return (S)e();
    }
  }

  static Storage e_storage() { return to_storage(e()); }

  static S call_op(const Storage& a, const Storage& b) {
    if constexpr (std::is_same_v<
                      std::remove_cvref_t<decltype(op(to_s(a), to_s(b)))>, S>) {
      return op(to_s(a), to_s(b));
    } else {
      return (S)op(to_s(a), to_s(b));
    }
  }

  static Storage op_storage(const Storage& a, const Storage& b) {
    return to_storage(op(to_s(a), to_s(b)));
  }
};

}  // namespace library3372::internal

#endif  // LIBRARY3372_LIBRARY3372_INTERNAL_ALGEBRA_MONOID_HPP_
