// Repository: Library3372
// File: library3372/internal/math/barrett.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/internal/math/barrett.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_INTERNAL_MATH_BARRETT_HPP_
#define LIBRARY3372_LIBRARY3372_INTERNAL_MATH_BARRETT_HPP_

#include <cassert>

namespace library3372::internal {

// 32-bit Barrett reduction for modulus 1 <= mod < 2^32
// https://en.wikipedia.org/wiki/Barrett_reduction
class Barrett32 {
 public:
  explicit constexpr Barrett32(unsigned int mod) : mod_(mod), im_(0) {
    assert(mod >= 1);
    if (mod > 1) im_ = (-((unsigned long long)mod)) / mod + 1ULL;
  }

  constexpr unsigned int mod() const { return mod_; }

  constexpr unsigned int reduce(unsigned long long z) const {
    if (mod_ == 1) return 0;
    unsigned long long x =
        (unsigned long long)(((unsigned __int128)z * im_) >> 64);
    unsigned long long v = z - x * mod_;
    return (unsigned int)(v >= mod_ ? v - mod_ : v);
  }

  constexpr unsigned int mul(unsigned int a, unsigned int b) const {
    return reduce((unsigned long long)a * b);
  }

 private:
  unsigned int mod_;
  unsigned long long im_;
};

}  // namespace library3372::internal

#endif  // LIBRARY3372_LIBRARY3372_INTERNAL_MATH_BARRETT_HPP_
