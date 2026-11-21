// Repository: Library3372
// File: library3372/math/static_modint.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/math/static_modint.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_MATH_STATIC_MODINT_HPP_
#define LIBRARY3372_LIBRARY3372_MATH_STATIC_MODINT_HPP_

#include <cassert>
#include <charconv>
#include <istream>
#include <ostream>
#include <string>
#include <utility>

#include "library3372/internal/type_traits/is_integer.hpp"

namespace library3372 {

template <int m> class StaticModInt {
  static_assert(m >= 1);

 public:
  static constexpr int mod() { return m; }

  template <typename T>
    requires internal::is_integer_v<T>
  static constexpr StaticModInt raw(T v) {
    StaticModInt res;
    res.val_ = (unsigned int)v;
    return res;
  }

  constexpr StaticModInt() : val_(0) {}

  template <typename T>
    requires internal::is_signed_integer_v<T>
  constexpr explicit StaticModInt(T v) {
    if constexpr (sizeof(T) <= sizeof(int)) {
      int x = (int)v % m;
      if (x < 0) x += m;
      val_ = (unsigned int)x;
    } else if constexpr (sizeof(T) <= sizeof(long long)) {
      long long x = (long long)v % m;
      if (x < 0) x += m;
      val_ = (unsigned int)x;
    } else {
      int x = (int)(v % (__int128)m);
      if (x < 0) x += m;
      val_ = (unsigned int)x;
    }
  }

  template <typename T>
    requires internal::is_unsigned_integer_v<T>
  constexpr explicit StaticModInt(T v) {
    if constexpr (sizeof(T) <= sizeof(unsigned int)) {
      val_ = (unsigned int)v % (unsigned int)m;
    } else if constexpr (sizeof(T) <= sizeof(unsigned long long)) {
      val_ = (unsigned int)((unsigned long long)v % (unsigned long long)m);
    } else {
      val_ = (unsigned int)(v % (unsigned __int128)m);
    }
  }

  constexpr int val() const { return (int)val_; }

  constexpr StaticModInt pow(long long n) const {
    assert(n >= 0);
    StaticModInt x = *this;
    StaticModInt res = raw(1 % m);
    while (n > 0) {
      if (n & 1) res *= x;
      x *= x;
      n >>= 1;
    }
    return res;
  }

  constexpr StaticModInt inv() const {
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

  constexpr StaticModInt operator+() const { return *this; }

  constexpr StaticModInt operator-() const {
    StaticModInt res;
    if (val_ != 0) res.val_ = (unsigned int)m - val_;
    return res;
  }

  constexpr StaticModInt& operator++() {
    ++val_;
    if (val_ == (unsigned int)m) val_ = 0;
    return *this;
  }

  constexpr StaticModInt operator++(int) {
    StaticModInt res = *this;
    ++*this;
    return res;
  }

  constexpr StaticModInt& operator--() {
    if (val_ == 0) val_ = (unsigned int)m;
    --val_;
    return *this;
  }

  constexpr StaticModInt operator--(int) {
    StaticModInt res = *this;
    --*this;
    return res;
  }

  constexpr StaticModInt& operator+=(StaticModInt rhs) {
    val_ += rhs.val_;
    if (val_ >= (unsigned int)m) val_ -= (unsigned int)m;
    return *this;
  }

  constexpr StaticModInt& operator-=(StaticModInt rhs) {
    if (val_ < rhs.val_) val_ += (unsigned int)m;
    val_ -= rhs.val_;
    return *this;
  }

  constexpr StaticModInt& operator*=(StaticModInt rhs) {
    val_ = (unsigned int)((unsigned long long)val_ * rhs.val_ %
                          (unsigned long long)m);
    return *this;
  }

  constexpr StaticModInt& operator/=(StaticModInt rhs) {
    return *this *= rhs.inv();
  }

  template <typename T>
    requires internal::is_integer_v<T>
  constexpr StaticModInt& operator+=(T rhs) {
    return *this += StaticModInt(rhs);
  }

  template <typename T>
    requires internal::is_integer_v<T>
  constexpr StaticModInt& operator-=(T rhs) {
    return *this -= StaticModInt(rhs);
  }

  template <typename T>
    requires internal::is_integer_v<T>
  constexpr StaticModInt& operator*=(T rhs) {
    return *this *= StaticModInt(rhs);
  }

  template <typename T>
    requires internal::is_integer_v<T>
  constexpr StaticModInt& operator/=(T rhs) {
    return *this /= StaticModInt(rhs);
  }

  friend constexpr StaticModInt operator+(StaticModInt lhs, StaticModInt rhs) {
    return lhs += rhs;
  }

  friend constexpr StaticModInt operator-(StaticModInt lhs, StaticModInt rhs) {
    return lhs -= rhs;
  }

  friend constexpr StaticModInt operator*(StaticModInt lhs, StaticModInt rhs) {
    return lhs *= rhs;
  }

  friend constexpr StaticModInt operator/(StaticModInt lhs, StaticModInt rhs) {
    return lhs /= rhs;
  }

  template <typename T>
    requires internal::is_integer_v<T>
  friend constexpr StaticModInt operator+(StaticModInt lhs, T rhs) {
    return lhs += rhs;
  }

  template <typename T>
    requires internal::is_integer_v<T>
  friend constexpr StaticModInt operator+(T lhs, StaticModInt rhs) {
    return StaticModInt(lhs) += rhs;
  }

  template <typename T>
    requires internal::is_integer_v<T>
  friend constexpr StaticModInt operator-(StaticModInt lhs, T rhs) {
    return lhs -= rhs;
  }

  template <typename T>
    requires internal::is_integer_v<T>
  friend constexpr StaticModInt operator-(T lhs, StaticModInt rhs) {
    return StaticModInt(lhs) -= rhs;
  }

  template <typename T>
    requires internal::is_integer_v<T>
  friend constexpr StaticModInt operator*(StaticModInt lhs, T rhs) {
    return lhs *= rhs;
  }

  template <typename T>
    requires internal::is_integer_v<T>
  friend constexpr StaticModInt operator*(T lhs, StaticModInt rhs) {
    return StaticModInt(lhs) *= rhs;
  }

  template <typename T>
    requires internal::is_integer_v<T>
  friend constexpr StaticModInt operator/(StaticModInt lhs, T rhs) {
    return lhs /= rhs;
  }

  template <typename T>
    requires internal::is_integer_v<T>
  friend constexpr StaticModInt operator/(T lhs, StaticModInt rhs) {
    return StaticModInt(lhs) /= rhs;
  }

  friend constexpr bool operator==(StaticModInt lhs, StaticModInt rhs) {
    return lhs.val_ == rhs.val_;
  }

  template <typename T>
    requires internal::is_integer_v<T>
  friend constexpr bool operator==(StaticModInt lhs, T rhs) {
    return lhs == StaticModInt(rhs);
  }

  friend std::ostream& operator<<(std::ostream& os, StaticModInt x) {
    return os << x.val_;
  }

  friend std::istream& operator>>(std::istream& is, StaticModInt& x) {
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
      x = StaticModInt(v);
    } else {
      if (s[0] == '+') ++begin;
      unsigned long long v = 0;
      [[maybe_unused]] std::from_chars_result res =
          std::from_chars(begin, end, v);
      assert(res.ec == std::errc() && res.ptr == end);
      x = StaticModInt(v);
    }
    return is;
  }

 private:
  unsigned int val_;
};

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_MATH_STATIC_MODINT_HPP_
