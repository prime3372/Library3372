// Repository: Library3372
// File: library3372/math/convolution.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/math/convolution.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_MATH_CONVOLUTION_HPP_
#define LIBRARY3372_LIBRARY3372_MATH_CONVOLUTION_HPP_

#include <algorithm>
#include <bit>
#include <cassert>
#include <type_traits>
#include <utility>
#include <vector>

#include "library3372/internal/math/ntt.hpp"
#include "library3372/internal/type_traits/is_integer.hpp"
#include "library3372/math/static_modint.hpp"

namespace library3372 {
namespace internal_convolution {

template <int m>
std::vector<unsigned int> convolution_naive(const unsigned int* a, int n,
                                            const unsigned int* b, int k) {
  if (n < k) {
    std::swap(a, b);
    std::swap(n, k);
  }
  int len = n + k - 1;
  std::vector<unsigned int> ans(len);
  unsigned int mod = (unsigned int)m;
  for (int idx = 0; idx < len; ++idx) {
    int lo = idx >= k - 1 ? idx - k + 1 : 0;
    int hi = idx < n ? idx : n - 1;
    unsigned __int128 sum = 0;
    for (int i = lo; i <= hi; ++i) sum += (unsigned long long)a[i] * b[idx - i];
    ans[idx] = (unsigned int)(sum % mod);
  }
  return ans;
}

template <int m, typename T>
void fill_raw_vector(const std::vector<T>& v, unsigned int* dst) {
  int n = (int)v.size();
  for (int i = 0; i < n; ++i) {
    dst[i] = (unsigned int)StaticModInt<m>(v[i]).val();
  }
}

template <int m>
void fill_raw_vector(const std::vector<StaticModInt<m>>& v, unsigned int* dst) {
  int n = (int)v.size();
  for (int i = 0; i < n; ++i) dst[i] = (unsigned int)v[i].val();
}

template <int m, typename Vec>
void fill_and_forward(const Vec& v, unsigned int* dst, int z) {
  fill_raw_vector<m>(v, dst);
  internal::ntt_forward_prepared<m>(dst, (int)v.size(), z);
}

template <int m, typename OutT, typename InT>
std::vector<OutT> convolution_impl(const std::vector<InT>& a,
                                   const std::vector<InT>& b) {
  assert(internal::NttInfo<m>::valid && m <= 2000000000);
  if (a.empty() || b.empty()) return {};
  int n = (int)a.size();
  int k = (int)b.size();
  long long s = (long long)n + k - 1;
  assert(s <= (1LL << internal::NttInfo<m>::rank2));

  auto to_out = [](unsigned int v) -> OutT {
    if constexpr (std::is_same_v<OutT, StaticModInt<m>>) {
      return StaticModInt<m>::raw(v);
    } else {
      return (OutT)v;
    }
  };

  bool same_input = (&a == &b);
  if (std::min(n, k) <= internal::ntt_naive_threshold) {
    internal::NttAlignedBuffer raw_a(n);
    fill_raw_vector<m>(a, raw_a.data);
    std::vector<unsigned int> raw_c;
    if (same_input) {
      raw_c = convolution_naive<m>(raw_a.data, n, raw_a.data, n);
    } else {
      internal::NttAlignedBuffer raw_b(k);
      fill_raw_vector<m>(b, raw_b.data);
      raw_c = convolution_naive<m>(raw_a.data, n, raw_b.data, k);
    }
    std::vector<OutT> res((int)s);
    for (int i = 0; i < (int)s; ++i) res[i] = to_out(raw_c[i]);
    return res;
  }

  int z = (int)std::bit_ceil((unsigned int)s);
  internal::NttAlignedBuffer raw_a(z);
  fill_and_forward<m>(a, raw_a.data, z);

  unsigned int factor = 0;
  if (same_input) {
    factor = internal::finish_ntt_convolution<m>(raw_a.data, nullptr, z, true);
  } else {
    internal::NttAlignedBuffer raw_b(z);
    fill_and_forward<m>(b, raw_b.data, z);
    factor =
        internal::finish_ntt_convolution<m>(raw_a.data, raw_b.data, z, false);
  }

  std::vector<OutT> res((int)s);
  if constexpr (std::is_same_v<OutT, unsigned int>) {
    internal::ntt_scale_out<m>(raw_a.data, res.data(), (int)s, factor);
  } else if constexpr (std::is_same_v<OutT, bool>) {
    internal::ntt_scale_out<m>(raw_a.data, raw_a.data, (int)s, factor);
    for (int i = 0; i < (int)s; ++i) res[i] = to_out(raw_a.data[i]);
  } else {
    internal::ntt_scale_out<m>(raw_a.data, res.data(), (int)s, factor, to_out);
  }
  return res;
}

}  // namespace internal_convolution

// Discrete convolution modulo m via Number Theoretic Transform (NTT)
// https://en.wikipedia.org/wiki/Number-theoretic_transform
template <int m>
std::vector<StaticModInt<m>> convolution(
    const std::vector<StaticModInt<m>>& a,
    const std::vector<StaticModInt<m>>& b) {
  return internal_convolution::convolution_impl<m, StaticModInt<m>>(a, b);
}

template <int m = 998244353, typename T>
  requires internal::is_integer_v<T>
std::vector<T> convolution(const std::vector<T>& a, const std::vector<T>& b) {
  return internal_convolution::convolution_impl<m, T>(a, b);
}

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_MATH_CONVOLUTION_HPP_
