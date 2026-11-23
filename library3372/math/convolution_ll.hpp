// Repository: Library3372
// File: library3372/math/convolution_ll.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/math/convolution_ll.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_MATH_CONVOLUTION_LL_HPP_
#define LIBRARY3372_LIBRARY3372_MATH_CONVOLUTION_LL_HPP_

#include <algorithm>
#include <bit>
#include <cassert>
#include <type_traits>
#include <utility>
#include <vector>

#include "library3372/internal/math/montgomery.hpp"
#include "library3372/internal/math/ntt.hpp"

namespace library3372 {
namespace internal_convolution_ll {

template <int m>
void fill_mod_and_forward(const std::vector<long long>& v, unsigned int* dst,
                          int z) {
  int n = (int)v.size();
  for (int i = 0; i < n; ++i) {
    long long x = v[i] % m;
    if (x < 0) x += m;
    dst[i] = (unsigned int)x;
  }
  internal::ntt_forward_prepared<m>(dst, n, z);
}

}  // namespace internal_convolution_ll

// 64-bit integer convolution via 2-prime NTT and Chinese Remainder Theorem
// https://en.wikipedia.org/wiki/Chinese_remainder_theorem
inline std::vector<long long> convolution_ll(const std::vector<long long>& a,
                                             const std::vector<long long>& b) {
  if (a.empty() || b.empty()) return {};
  long long s = (long long)a.size() + (long long)b.size() - 1;
  assert(s <= (1LL << 24));

  int n = (int)a.size();
  int k = (int)b.size();
  if (std::min(n, k) <= internal::ntt_naive_threshold) {
    const long long* pa = a.data();
    const long long* pb = b.data();
    if (n < k) {
      std::swap(pa, pb);
      std::swap(n, k);
    }
    std::vector<long long> ans((int)s);
    for (int idx = 0; idx < (int)s; ++idx) {
      int lo = idx >= k - 1 ? idx - k + 1 : 0;
      int hi = idx < n ? idx : n - 1;
      unsigned long long sum = 0;
      for (int i = lo; i <= hi; ++i) {
        sum += (unsigned long long)pa[i] * (unsigned long long)pb[idx - i];
      }
      ans[idx] = (long long)sum;
    }
    return ans;
  }

  constexpr int m1 = 2130706433;
  constexpr int m2 = 2113929217;
  constexpr unsigned long long m12 = (unsigned long long)m1 * m2;
  constexpr unsigned long long m12_half = m12 >> 1;
  constexpr internal::Montgomery32 mont2((unsigned int)m2);
  constexpr unsigned int inv12 = mont2.from_mont(
      mont2.pow(mont2.to_mont((unsigned int)m1 % (unsigned int)m2), m2 - 2));

  bool same_input = (&a == &b);
  int z = (int)std::bit_ceil((unsigned int)s);
  internal::NttAlignedBuffer ra(z);
  internal::NttAlignedBuffer rb(same_input ? 0 : z);

  auto run = [&](auto tag, unsigned int* out) {
    constexpr int mod = decltype(tag)::value;
    internal_convolution_ll::fill_mod_and_forward<mod>(a, ra.data, z);
    if (!same_input) {
      internal_convolution_ll::fill_mod_and_forward<mod>(b, rb.data, z);
    }
    unsigned int factor = internal::finish_ntt_convolution<mod>(
        ra.data, same_input ? nullptr : rb.data, z, same_input);
    internal::ntt_scale_out<mod>(ra.data, out, (int)s, factor);
  };

  internal::NttAlignedBuffer c1((int)s);
  run(std::integral_constant<int, m1>{}, c1.data);
  run(std::integral_constant<int, m2>{}, ra.data);

  std::vector<long long> c((int)s);
  for (int i = 0; i < (int)s; ++i) {
    unsigned long long r1 = c1.data[i];
    unsigned long long r2 = ra.data[i];
    unsigned long long r1_mod_m2 =
        r1 >= (unsigned long long)m2 ? r1 - (unsigned long long)m2 : r1;
    unsigned long long diff = r2 >= r1_mod_m2
                                  ? r2 - r1_mod_m2
                                  : r2 + (unsigned long long)m2 - r1_mod_m2;
    unsigned long long k2 = diff * inv12 % (unsigned long long)m2;
    unsigned long long x = r1 + (unsigned long long)m1 * k2;
    if (x > m12_half) x -= m12;
    c[i] = (long long)x;
  }
  return c;
}

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_MATH_CONVOLUTION_LL_HPP_
