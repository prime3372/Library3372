// Repository: Library3372
// File: library3372/math/convolution_arbitrary_mod.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/math/convolution_arbitrary_mod.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_MATH_CONVOLUTION_ARBITRARY_MOD_HPP_
#define LIBRARY3372_LIBRARY3372_MATH_CONVOLUTION_ARBITRARY_MOD_HPP_

#include <algorithm>
#include <cassert>
#include <type_traits>
#include <utility>
#include <vector>

#include "library3372/internal/math/barrett.hpp"
#include "library3372/internal/math/convolution_crt.hpp"
#include "library3372/internal/math/ntt.hpp"
#include "library3372/internal/type_traits/is_integer.hpp"
#include "library3372/math/dynamic_modint.hpp"
#include "library3372/math/static_modint.hpp"

namespace library3372 {
namespace internal_convolution_arbitrary_mod {

template <typename T>
unsigned int normalize_elem32(T v, const internal::Barrett32& barrett) {
  if constexpr (internal::is_signed_integer_v<T>) {
    if constexpr (sizeof(T) <= sizeof(long long)) {
      long long x = (long long)v;
      if (x >= 0) return barrett.reduce((unsigned long long)x);
      unsigned int r = barrett.reduce(0ULL - (unsigned long long)x);
      return r == 0 ? 0U : barrett.mod() - r;
    } else {
      int m = (int)barrett.mod();
      int x = (int)(v % (__int128)m);
      if (x < 0) x += m;
      return (unsigned int)x;
    }
  } else {
    if constexpr (sizeof(T) <= sizeof(unsigned long long)) {
      return barrett.reduce((unsigned long long)v);
    } else {
      return (unsigned int)(v % (unsigned __int128)barrett.mod());
    }
  }
}

template <typename T> unsigned long long normalize_elem64(T v, long long mod) {
  if constexpr (internal::is_signed_integer_v<T>) {
    if constexpr (sizeof(T) <= sizeof(long long)) {
      long long x = (long long)v % mod;
      if (x < 0) x += mod;
      return (unsigned long long)x;
    } else {
      long long x = (long long)(v % (__int128)mod);
      if (x < 0) x += mod;
      return (unsigned long long)x;
    }
  } else {
    if constexpr (sizeof(T) <= sizeof(unsigned long long)) {
      return (unsigned long long)v % (unsigned long long)mod;
    } else {
      return (unsigned long long)(v % (unsigned __int128)mod);
    }
  }
}

template <typename OutT, typename ElemT, typename ToOut>
std::vector<OutT> convolution_naive32(const ElemT* pa, int n, const ElemT* pb,
                                      int k, unsigned int mod, ToOut to_out) {
  if (n < k) {
    std::swap(pa, pb);
    std::swap(n, k);
  }
  int len = n + k - 1;
  std::vector<OutT> ans(len);
  for (int idx = 0; idx < len; ++idx) {
    int lo = idx >= k - 1 ? idx - k + 1 : 0;
    int hi = idx < n ? idx : n - 1;
    unsigned __int128 sum = 0;
    for (int i = lo; i <= hi; ++i) {
      if constexpr (std::is_same_v<std::remove_cv_t<ElemT>, unsigned int>) {
        sum += (unsigned long long)pa[i] * pb[idx - i];
      } else {
        sum += (unsigned long long)(unsigned int)pa[i].val() *
               (unsigned int)pb[idx - i].val();
      }
    }
    ans[idx] = to_out((unsigned int)(sum % mod));
  }
  return ans;
}

template <typename T>
std::vector<T> convolution_naive64(const unsigned long long* pa, int n,
                                   const unsigned long long* pb, int k,
                                   unsigned long long mod) {
  if (n < k) {
    std::swap(pa, pb);
    std::swap(n, k);
  }
  int len = n + k - 1;
  std::vector<T> ans(len);
  constexpr unsigned __int128 limit = (unsigned __int128)1 << 126;
  for (int idx = 0; idx < len; ++idx) {
    int lo = idx >= k - 1 ? idx - k + 1 : 0;
    int hi = idx < n ? idx : n - 1;
    unsigned __int128 sum = 0;
    for (int i = lo; i <= hi; ++i) {
      sum += (unsigned __int128)pa[i] * pb[idx - i];
      if (sum >= limit) sum %= mod;
    }
    ans[idx] = (T)(unsigned long long)(sum % mod);
  }
  return ans;
}

template <typename OutT, typename InT>
std::vector<OutT> convolution_arbitrary_mod32_impl(const std::vector<InT>& a,
                                                   const std::vector<InT>& b,
                                                   int mod) {
  assert(mod >= 1);
  if (a.empty() || b.empty()) return {};
  int n = (int)a.size();
  int k = (int)b.size();
  long long s = (long long)n + k - 1;
  assert(s <= (1LL << 24));

  auto to_out = [](unsigned int v) -> OutT {
    if constexpr (internal::is_integer_v<OutT>) {
      return (OutT)v;
    } else {
      return OutT::raw(v);
    }
  };

  if (mod == 1) return std::vector<OutT>((int)s, to_out(0));

  internal::Barrett32 barrett((unsigned int)mod);
  bool same_input = (&a == &b);

  internal::NttAlignedBuffer norm_a(internal::is_integer_v<InT> ? n : 0);
  internal::NttAlignedBuffer norm_b(
      (internal::is_integer_v<InT> && !same_input) ? k : 0);
  if constexpr (internal::is_integer_v<InT>) {
    for (int i = 0; i < n; ++i)
      norm_a.data[i] = normalize_elem32(a[i], barrett);
    if (!same_input) {
      for (int i = 0; i < k; ++i) {
        norm_b.data[i] = normalize_elem32(b[i], barrett);
      }
    }
  }

  auto get_src_a = [&]() {
    if constexpr (internal::is_integer_v<InT>) {
      return (const unsigned int*)norm_a.data;
    } else {
      return a.data();
    }
  };
  auto get_src_b = [&]() {
    if constexpr (internal::is_integer_v<InT>) {
      return (const unsigned int*)(same_input ? norm_a.data : norm_b.data);
    } else {
      return (same_input ? a.data() : b.data());
    }
  };

  if (std::min(n, k) <= internal::ntt_naive_threshold) {
    return convolution_naive32<OutT>(get_src_a(), n, get_src_b(), k,
                                     (unsigned int)mod, to_out);
  }

  constexpr int m0 = 167772161;
  constexpr int m1 = 469762049;
  constexpr int m2 = 754974721;
  unsigned int m0_mod = barrett.reduce((unsigned long long)m0);
  unsigned int m01_mod = barrett.reduce((unsigned long long)m0 * m1);

  std::vector<OutT> res((int)s);
  const auto* pa = get_src_a();
  const auto* pb = get_src_b();
  auto read_elem = [](const auto* ptr, int i) -> unsigned int {
    if constexpr (internal::is_integer_v<InT>) {
      return ptr[i];
    } else {
      return (unsigned int)ptr[i].val();
    }
  };
  internal::convolution_crt<m0, m1, m2>(
      [&](int i) { return read_elem(pa, i); }, n,
      [&](int i) { return read_elem(pb, i); }, k, same_input,
      [&](int i, const unsigned int* x) {
        unsigned long long acc = x[0] + (unsigned long long)m0_mod * x[1] +
                                 (unsigned long long)m01_mod * x[2];
        res[i] = to_out(barrett.reduce(acc));
      });
  return res;
}

template <typename T>
std::vector<T> convolution_arbitrary_mod64_impl(const std::vector<T>& a,
                                                const std::vector<T>& b,
                                                long long mod) {
  assert(mod >= 1);
  if (a.empty() || b.empty()) return {};
  int n = (int)a.size();
  int k = (int)b.size();
  long long s = (long long)n + k - 1;
  assert(s <= (1LL << 24));
  if (mod == 1) return std::vector<T>((int)s, (T)0);

  bool same_input = (&a == &b);
  internal::NttAlignedBuffer64 norm_a(n);
  for (int i = 0; i < n; ++i) norm_a.data[i] = normalize_elem64(a[i], mod);
  internal::NttAlignedBuffer64 norm_b(same_input ? 0 : k);
  if (!same_input) {
    for (int i = 0; i < k; ++i) norm_b.data[i] = normalize_elem64(b[i], mod);
  }

  unsigned long long um = (unsigned long long)mod;
  if (std::min(n, k) <= internal::ntt_naive_threshold) {
    return convolution_naive64<T>(
        norm_a.data, n, same_input ? norm_a.data : norm_b.data, k, um);
  }

  constexpr int m0 = 2130706433;
  constexpr int m1 = 2113929217;
  constexpr int m2 = 2013265921;
  constexpr int m3 = 1811939329;
  constexpr int m4 = 1711276033;
  constexpr unsigned long long m01_u64 = (unsigned long long)m0 * m1;
  unsigned long long m01_mod = m01_u64 % um;
  unsigned long long m012_mod =
      (unsigned long long)((unsigned __int128)m01_mod * m2 % um);
  unsigned long long m0123_mod =
      (unsigned long long)((unsigned __int128)m012_mod * m3 % um);

  std::vector<T> res((int)s);
  const unsigned long long* pb_ptr = same_input ? norm_a.data : norm_b.data;
  internal::convolution_crt<m0, m1, m2, m3, m4>(
      [&](int i) { return norm_a.data[i]; }, n,
      [&](int i) { return pb_ptr[i]; }, k, same_input,
      [&](int i, const unsigned int* x) {
        unsigned __int128 acc = x[0];
        acc += (unsigned __int128)(unsigned int)m0 * x[1];
        acc += (unsigned __int128)m01_mod * x[2];
        acc += (unsigned __int128)m012_mod * x[3];
        acc += (unsigned __int128)m0123_mod * x[4];
        res[i] = (T)(unsigned long long)(acc % um);
      });
  return res;
}

}  // namespace internal_convolution_arbitrary_mod

template <int m>
std::vector<StaticModInt<m>> convolution_arbitrary_mod(
    const std::vector<StaticModInt<m>>& a,
    const std::vector<StaticModInt<m>>& b) {
  return internal_convolution_arbitrary_mod::convolution_arbitrary_mod32_impl<
      StaticModInt<m>>(a, b, m);
}

template <int id>
std::vector<DynamicModInt<id>> convolution_arbitrary_mod(
    const std::vector<DynamicModInt<id>>& a,
    const std::vector<DynamicModInt<id>>& b) {
  return internal_convolution_arbitrary_mod::convolution_arbitrary_mod32_impl<
      DynamicModInt<id>>(a, b, DynamicModInt<id>::mod());
}

template <typename T>
  requires internal::is_integer_v<T>
std::vector<T> convolution_arbitrary_mod(const std::vector<T>& a,
                                         const std::vector<T>& b,
                                         long long mod) {
  assert(mod >= 1);
  if (mod <= 2147483647LL) {
    return internal_convolution_arbitrary_mod::convolution_arbitrary_mod32_impl<
        T>(a, b, (int)mod);
  }
  return internal_convolution_arbitrary_mod::convolution_arbitrary_mod64_impl<
      T>(a, b, mod);
}

template <long long m, typename T>
  requires internal::is_integer_v<T>
std::vector<T> convolution_arbitrary_mod(const std::vector<T>& a,
                                         const std::vector<T>& b) {
  static_assert(m >= 1);
  if constexpr (m <= 2147483647LL) {
    return internal_convolution_arbitrary_mod::convolution_arbitrary_mod32_impl<
        T>(a, b, (int)m);
  } else {
    return internal_convolution_arbitrary_mod::convolution_arbitrary_mod64_impl<
        T>(a, b, m);
  }
}

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_MATH_CONVOLUTION_ARBITRARY_MOD_HPP_
