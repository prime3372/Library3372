// Repository: Library3372
// File: library3372/internal/algebra/acted_monoid.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/internal/algebra/acted_monoid.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_INTERNAL_ALGEBRA_ACTED_MONOID_HPP_
#define LIBRARY3372_LIBRARY3372_INTERNAL_ALGEBRA_ACTED_MONOID_HPP_

#include <concepts>

#include "library3372/internal/algebra/monoid.hpp"

namespace library3372::internal {

template <typename S, auto op, auto e, typename F, auto mapping,
          auto composition, auto id>
concept ActedMonoid =
    Monoid<S, op, e> && Monoid<F, composition, id> &&
    is_non_null_callable<mapping>() && requires(const F& f, const S& x) {
      { mapping(f, x) } -> std::convertible_to<S>;
    };

template <typename S, auto op, auto e, typename F, auto mapping,
          auto composition, auto id>
  requires ActedMonoid<S, op, e, F, mapping, composition, id>
struct ActedMonoidAdapter {
  using SAdapter = MonoidAdapter<S, op, e>;
  using FAdapter = MonoidAdapter<F, composition, id>;
  using SStorage = typename SAdapter::Storage;
  using FStorage = typename FAdapter::Storage;

  static SStorage mapping_storage(const FStorage& f, const SStorage& x) {
    return SAdapter::to_storage(mapping(FAdapter::to_s(f), SAdapter::to_s(x)));
  }
};

}  // namespace library3372::internal

#endif  // LIBRARY3372_LIBRARY3372_INTERNAL_ALGEBRA_ACTED_MONOID_HPP_
