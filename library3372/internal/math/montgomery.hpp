// Repository: Library3372
// File: library3372/internal/math/montgomery.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/internal/math/montgomery.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_INTERNAL_MATH_MONTGOMERY_HPP_
#define LIBRARY3372_LIBRARY3372_INTERNAL_MATH_MONTGOMERY_HPP_

#include <cassert>

namespace library3372::internal {

// 64-bit Montgomery arithmetic for odd modulus 1 <= mod < 2^64
// https://en.wikipedia.org/wiki/Montgomery_modular_multiplication
class Montgomery64 {
 public:
  explicit constexpr Montgomery64(unsigned long long mod)
      : mod_(mod), inv_(mod), one_(0), r2_(0) {
    assert((mod & 1ULL) == 1ULL);
    one_ = (-mod_) % mod_;
    for (int i = 0; i < 5; ++i) inv_ *= 2ULL - mod_ * inv_;
    r2_ = add(one_, one_);
    for (int i = 0; i < 6; ++i) r2_ = mul(r2_, r2_);
  }

  constexpr unsigned long long mod() const { return mod_; }
  constexpr unsigned long long one() const { return one_; }
  constexpr unsigned long long r2() const { return r2_; }

  constexpr unsigned long long to_mont(unsigned long long x) const {
    return reduce((unsigned __int128)x * r2_);
  }

  constexpr unsigned long long from_mont(unsigned long long x) const {
    return reduce(x);
  }

  constexpr unsigned long long add(unsigned long long a,
                                   unsigned long long b) const {
    return b >= mod_ - a ? a - (mod_ - b) : a + b;
  }

  constexpr unsigned long long sub(unsigned long long a,
                                   unsigned long long b) const {
    return a >= b ? a - b : mod_ - b + a;
  }

  constexpr unsigned long long neg(unsigned long long a) const {
    return a == 0 ? 0 : mod_ - a;
  }

  constexpr unsigned long long mul(unsigned long long a,
                                   unsigned long long b) const {
    return reduce((unsigned __int128)a * b);
  }

  constexpr unsigned long long pow(unsigned long long base,
                                   unsigned long long exp) const {
    unsigned long long res = one_;
    while (exp > 0) {
      if (exp & 1ULL) res = mul(res, base);
      base = mul(base, base);
      exp >>= 1;
    }
    return res;
  }

 private:
  constexpr unsigned long long reduce(unsigned __int128 t) const {
    unsigned long long m = (unsigned long long)t * inv_;
    unsigned long long high_t = (unsigned long long)(t >> 64);
    unsigned long long high_mn =
        (unsigned long long)(((unsigned __int128)m * mod_) >> 64);
    return high_t < high_mn ? high_t - high_mn + mod_ : high_t - high_mn;
  }

  unsigned long long mod_;
  unsigned long long inv_;
  unsigned long long one_;
  unsigned long long r2_;
};

}  // namespace library3372::internal

#endif  // LIBRARY3372_LIBRARY3372_INTERNAL_MATH_MONTGOMERY_HPP_
