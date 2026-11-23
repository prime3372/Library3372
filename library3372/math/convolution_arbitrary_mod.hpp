// Repository: Library3372
// File: library3372/math/convolution_arbitrary_mod.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/math/convolution_arbitrary_mod.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_MATH_CONVOLUTION_ARBITRARY_MOD_HPP_
#define LIBRARY3372_LIBRARY3372_MATH_CONVOLUTION_ARBITRARY_MOD_HPP_

#include <algorithm>
#include <bit>
#include <cassert>
#include <type_traits>
#include <utility>
#include <vector>

#include "library3372/internal/math/barrett.hpp"
#include "library3372/internal/math/montgomery.hpp"
#include "library3372/internal/math/ntt.hpp"
#include "library3372/internal/type_traits/is_integer.hpp"
#include "library3372/math/dynamic_modint.hpp"
#include "library3372/math/static_modint.hpp"

namespace library3372 {
namespace internal_convolution_arbitrary_mod {

template <typename T>
unsigned int normalize_elem(T v, const internal::Barrett32& barrett) {
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

template <typename T>
void fill_normalized(const std::vector<T>& v, unsigned int* dst,
                     const internal::Barrett32& barrett) {
  int n = (int)v.size();
  for (int i = 0; i < n; ++i) dst[i] = normalize_elem(v[i], barrett);
}

template <int prime, typename ElemT>
void fill_and_forward_prime(const ElemT* src, int n, unsigned int* dst, int z) {
  for (int i = 0; i < n; ++i) {
    if constexpr (std::is_same_v<ElemT, unsigned int>) {
      dst[i] = src[i] % (unsigned int)prime;
    } else {
      dst[i] = (unsigned int)src[i].val() % (unsigned int)prime;
    }
  }
  internal::ntt_forward_prepared<prime>(dst, n, z);
}

template <typename OutT, typename ElemT, typename ToOut>
std::vector<OutT> convolution_naive(const ElemT* pa, int n, const ElemT* pb,
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

template <typename OutT, typename InT>
std::vector<OutT> convolution_arbitrary_mod_impl(const std::vector<InT>& a,
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
    fill_normalized(a, norm_a.data, barrett);
    if (!same_input) fill_normalized(b, norm_b.data, barrett);
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
    return convolution_naive<OutT>(get_src_a(), n, get_src_b(), k,
                                   (unsigned int)mod, to_out);
  }

  // 3-prime NTT + Garner's CRT reconstruction:
  // https://en.wikipedia.org/wiki/Chinese_remainder_theorem
  constexpr int m1 = 167772161;
  constexpr int m2 = 469762049;
  constexpr int m3 = 754974721;
  constexpr internal::Montgomery32 mont2((unsigned int)m2);
  constexpr internal::Montgomery32 mont3((unsigned int)m3);
  constexpr internal::Barrett32 barrett3((unsigned int)m3);
  constexpr unsigned int inv12_mont =
      mont2.pow(mont2.to_mont((unsigned int)m1), m2 - 2);
  constexpr unsigned int m12_mod_m3 =
      (unsigned int)((unsigned long long)m1 * m2 % (unsigned long long)m3);
  constexpr unsigned int inv12_3_mont =
      mont3.pow(mont3.to_mont(m12_mod_m3), m3 - 2);
  unsigned int m12_mod = barrett.reduce((unsigned long long)m1 * m2);

  int z = (int)std::bit_ceil((unsigned int)s);
  internal::NttAlignedBuffer ra(z);
  internal::NttAlignedBuffer rb(same_input ? 0 : z);

  auto run = [&](auto tag, unsigned int* out) {
    constexpr int prime = decltype(tag)::value;
    fill_and_forward_prime<prime>(get_src_a(), n, ra.data, z);
    if (!same_input) {
      fill_and_forward_prime<prime>(get_src_b(), k, rb.data, z);
    }
    unsigned int factor = internal::finish_ntt_convolution<prime>(
        ra.data, same_input ? nullptr : rb.data, z, same_input);
    internal::ntt_scale_out<prime>(ra.data, out, (int)s, factor);
  };

  internal::NttAlignedBuffer c1((int)s);
  internal::NttAlignedBuffer c2((int)s);
  run(std::integral_constant<int, m1>{}, c1.data);
  run(std::integral_constant<int, m2>{}, c2.data);
  run(std::integral_constant<int, m3>{}, ra.data);

  std::vector<OutT> res((int)s);
  for (int i = 0; i < (int)s; ++i) {
    unsigned int r1 = c1.data[i];
    unsigned int r2 = c2.data[i];
    unsigned int r3 = ra.data[i];
    unsigned int diff2 = r2 >= r1 ? r2 - r1 : r2 + (unsigned int)m2 - r1;
    unsigned int x2 = mont2.mul(diff2, inv12_mont);
    unsigned long long v12 = r1 + (unsigned long long)m1 * x2;
    unsigned int v12_mod_m3 = barrett3.reduce(v12);
    unsigned int diff3 =
        r3 >= v12_mod_m3 ? r3 - v12_mod_m3 : r3 + (unsigned int)m3 - v12_mod_m3;
    unsigned int x3 = mont3.mul(diff3, inv12_3_mont);
    unsigned long long v12_mod = barrett.reduce(v12);
    unsigned int ans =
        barrett.reduce(v12_mod + (unsigned long long)m12_mod * x3);
    res[i] = to_out(ans);
  }
  return res;
}

}  // namespace internal_convolution_arbitrary_mod

template <int m>
std::vector<StaticModInt<m>> convolution_arbitrary_mod(
    const std::vector<StaticModInt<m>>& a,
    const std::vector<StaticModInt<m>>& b) {
  return internal_convolution_arbitrary_mod::convolution_arbitrary_mod_impl<
      StaticModInt<m>>(a, b, m);
}

template <int id>
std::vector<DynamicModInt<id>> convolution_arbitrary_mod(
    const std::vector<DynamicModInt<id>>& a,
    const std::vector<DynamicModInt<id>>& b) {
  return internal_convolution_arbitrary_mod::convolution_arbitrary_mod_impl<
      DynamicModInt<id>>(a, b, DynamicModInt<id>::mod());
}

template <typename T>
  requires internal::is_integer_v<T>
std::vector<T> convolution_arbitrary_mod(const std::vector<T>& a,
                                         const std::vector<T>& b, int mod) {
  return internal_convolution_arbitrary_mod::convolution_arbitrary_mod_impl<T>(
      a, b, mod);
}

template <int m, typename T>
  requires internal::is_integer_v<T>
std::vector<T> convolution_arbitrary_mod(const std::vector<T>& a,
                                         const std::vector<T>& b) {
  static_assert(m >= 1);
  return internal_convolution_arbitrary_mod::convolution_arbitrary_mod_impl<T>(
      a, b, m);
}

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_MATH_CONVOLUTION_ARBITRARY_MOD_HPP_
