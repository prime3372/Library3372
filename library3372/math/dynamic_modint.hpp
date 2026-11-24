// Repository: Library3372
// File: library3372/math/dynamic_modint.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/math/dynamic_modint.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_MATH_DYNAMIC_MODINT_HPP_
#define LIBRARY3372_LIBRARY3372_MATH_DYNAMIC_MODINT_HPP_

#include <cassert>
#include <istream>
#include <ostream>

#include "library3372/internal/math/barrett.hpp"
#include "library3372/internal/math/modint_common.hpp"
#include "library3372/internal/type_traits/is_integer.hpp"

namespace library3372 {

template <int id> class DynamicModInt {
 public:
  static int mod() { return (int)barrett_.mod(); }

  static void set_mod(int m) {
    assert(m >= 1);
    barrett_ = internal::Barrett32((unsigned int)m);
  }

  template <typename T>
    requires internal::is_integer_v<T>
  static DynamicModInt raw(T v) {
    DynamicModInt res;
    res.val_ = (unsigned int)v;
    return res;
  }

  DynamicModInt() : val_(0) {}

  template <typename T>
    requires internal::is_signed_integer_v<T>
  DynamicModInt(T v) {
    if constexpr (sizeof(T) <= sizeof(long long)) {
      long long x = (long long)v;
      if (x >= 0) {
        val_ = barrett_.reduce((unsigned long long)x);
      } else {
        unsigned int r = barrett_.reduce(0ULL - (unsigned long long)x);
        val_ = (r == 0 ? 0U : barrett_.mod() - r);
      }
    } else {
      int x = (int)(v % (__int128)mod());
      if (x < 0) x += mod();
      val_ = (unsigned int)x;
    }
  }

  template <typename T>
    requires internal::is_unsigned_integer_v<T>
  DynamicModInt(T v) {
    if constexpr (sizeof(T) <= sizeof(unsigned long long)) {
      val_ = barrett_.reduce((unsigned long long)v);
    } else {
      val_ = (unsigned int)(v % (unsigned __int128)barrett_.mod());
    }
  }

  int val() const { return (int)val_; }

  DynamicModInt pow(long long n) const {
    return internal::modint_pow(*this, n);
  }

  DynamicModInt inv() const {
    return internal::modint_inv<DynamicModInt>((int)val_, mod());
  }

  DynamicModInt operator+() const { return *this; }

  DynamicModInt operator-() const {
    DynamicModInt res;
    if (val_ != 0) res.val_ = barrett_.mod() - val_;
    return res;
  }

  DynamicModInt& operator++() {
    ++val_;
    if (val_ == barrett_.mod()) val_ = 0;
    return *this;
  }

  DynamicModInt operator++(int) {
    DynamicModInt res = *this;
    ++*this;
    return res;
  }

  DynamicModInt& operator--() {
    if (val_ == 0) val_ = barrett_.mod();
    --val_;
    return *this;
  }

  DynamicModInt operator--(int) {
    DynamicModInt res = *this;
    --*this;
    return res;
  }

  DynamicModInt& operator+=(DynamicModInt rhs) {
    val_ += rhs.val_;
    if (val_ >= barrett_.mod()) val_ -= barrett_.mod();
    return *this;
  }

  DynamicModInt& operator-=(DynamicModInt rhs) {
    if (val_ < rhs.val_) val_ += barrett_.mod();
    val_ -= rhs.val_;
    return *this;
  }

  DynamicModInt& operator*=(DynamicModInt rhs) {
    val_ = barrett_.mul(val_, rhs.val_);
    return *this;
  }

  DynamicModInt& operator/=(DynamicModInt rhs) { return *this *= rhs.inv(); }

  friend DynamicModInt operator+(DynamicModInt lhs, DynamicModInt rhs) {
    return lhs += rhs;
  }

  friend DynamicModInt operator-(DynamicModInt lhs, DynamicModInt rhs) {
    return lhs -= rhs;
  }

  friend DynamicModInt operator*(DynamicModInt lhs, DynamicModInt rhs) {
    return lhs *= rhs;
  }

  friend DynamicModInt operator/(DynamicModInt lhs, DynamicModInt rhs) {
    return lhs /= rhs;
  }

  friend bool operator==(DynamicModInt lhs, DynamicModInt rhs) {
    return lhs.val_ == rhs.val_;
  }

  friend std::ostream& operator<<(std::ostream& os, DynamicModInt x) {
    return os << x.val_;
  }

  friend std::istream& operator>>(std::istream& is, DynamicModInt& x) {
    return internal::modint_istream(is, x);
  }

 private:
  unsigned int val_;
  static inline internal::Barrett32 barrett_{998244353U};
};

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_MATH_DYNAMIC_MODINT_HPP_
