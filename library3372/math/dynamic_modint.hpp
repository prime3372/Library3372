// Repository: Library3372
// File: library3372/math/dynamic_modint.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/math/dynamic_modint.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_MATH_DYNAMIC_MODINT_HPP_
#define LIBRARY3372_LIBRARY3372_MATH_DYNAMIC_MODINT_HPP_

#include <cassert>
#include <charconv>
#include <istream>
#include <ostream>
#include <string>
#include <utility>

#include "library3372/internal/math/barrett.hpp"
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
  explicit DynamicModInt(T v) {
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
  explicit DynamicModInt(T v) {
    if constexpr (sizeof(T) <= sizeof(unsigned long long)) {
      val_ = barrett_.reduce((unsigned long long)v);
    } else {
      val_ = (unsigned int)(v % (unsigned __int128)barrett_.mod());
    }
  }

  int val() const { return (int)val_; }

  DynamicModInt pow(long long n) const {
    assert(n >= 0);
    DynamicModInt x = *this;
    DynamicModInt res(1);
    while (n > 0) {
      if (n & 1) res *= x;
      x *= x;
      n >>= 1;
    }
    return res;
  }

  DynamicModInt inv() const {
    int m = mod();
    int s = m;
    int t = (int)val_;
    int m0 = 0;
    int m1 = 1;
    while (t != 0) {
      int u = s / t;
      s -= t * u;
      m0 -= m1 * u;
      std::swap(s, t);
      std::swap(m0, m1);
    }
    assert(s == 1);
    if (m0 < 0) m0 += m;
    return raw(m0);
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

  template <typename T>
    requires internal::is_integer_v<T>
  DynamicModInt& operator+=(T rhs) {
    return *this += DynamicModInt(rhs);
  }

  template <typename T>
    requires internal::is_integer_v<T>
  DynamicModInt& operator-=(T rhs) {
    return *this -= DynamicModInt(rhs);
  }

  template <typename T>
    requires internal::is_integer_v<T>
  DynamicModInt& operator*=(T rhs) {
    return *this *= DynamicModInt(rhs);
  }

  template <typename T>
    requires internal::is_integer_v<T>
  DynamicModInt& operator/=(T rhs) {
    return *this /= DynamicModInt(rhs);
  }

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

  template <typename T>
    requires internal::is_integer_v<T>
  friend DynamicModInt operator+(DynamicModInt lhs, T rhs) {
    return lhs += rhs;
  }

  template <typename T>
    requires internal::is_integer_v<T>
  friend DynamicModInt operator+(T lhs, DynamicModInt rhs) {
    return DynamicModInt(lhs) += rhs;
  }

  template <typename T>
    requires internal::is_integer_v<T>
  friend DynamicModInt operator-(DynamicModInt lhs, T rhs) {
    return lhs -= rhs;
  }

  template <typename T>
    requires internal::is_integer_v<T>
  friend DynamicModInt operator-(T lhs, DynamicModInt rhs) {
    return DynamicModInt(lhs) -= rhs;
  }

  template <typename T>
    requires internal::is_integer_v<T>
  friend DynamicModInt operator*(DynamicModInt lhs, T rhs) {
    return lhs *= rhs;
  }

  template <typename T>
    requires internal::is_integer_v<T>
  friend DynamicModInt operator*(T lhs, DynamicModInt rhs) {
    return DynamicModInt(lhs) *= rhs;
  }

  template <typename T>
    requires internal::is_integer_v<T>
  friend DynamicModInt operator/(DynamicModInt lhs, T rhs) {
    return lhs /= rhs;
  }

  template <typename T>
    requires internal::is_integer_v<T>
  friend DynamicModInt operator/(T lhs, DynamicModInt rhs) {
    return DynamicModInt(lhs) /= rhs;
  }

  friend bool operator==(DynamicModInt lhs, DynamicModInt rhs) {
    return lhs.val_ == rhs.val_;
  }

  template <typename T>
    requires internal::is_integer_v<T>
  friend bool operator==(DynamicModInt lhs, T rhs) {
    return lhs == DynamicModInt(rhs);
  }

  friend std::ostream& operator<<(std::ostream& os, DynamicModInt x) {
    return os << x.val_;
  }

  friend std::istream& operator>>(std::istream& is, DynamicModInt& x) {
    std::string s;
    is >> s;
    assert((bool)is);
    const char* begin = s.data();
    const char* end = begin + s.size();
    if (s[0] == '-') {
      long long v = 0;
      [[maybe_unused]] std::from_chars_result res =
          std::from_chars(begin, end, v);
      assert(res.ec == std::errc() && res.ptr == end);
      x = DynamicModInt(v);
    } else {
      if (s[0] == '+') ++begin;
      unsigned long long v = 0;
      [[maybe_unused]] std::from_chars_result res =
          std::from_chars(begin, end, v);
      assert(res.ec == std::errc() && res.ptr == end);
      x = DynamicModInt(v);
    }
    return is;
  }

 private:
  unsigned int val_;
  static inline internal::Barrett32 barrett_{998244353U};
};

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_MATH_DYNAMIC_MODINT_HPP_
