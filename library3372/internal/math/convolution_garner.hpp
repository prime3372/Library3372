// Repository: Library3372
// File: library3372/internal/math/convolution_garner.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/internal/math/convolution_garner.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_INTERNAL_MATH_CONVOLUTION_GARNER_HPP_
#define LIBRARY3372_LIBRARY3372_INTERNAL_MATH_CONVOLUTION_GARNER_HPP_

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

namespace library3372::internal {

template <int... Primes> struct CrtInfo {
  static constexpr int num_primes = sizeof...(Primes);
  static_assert(num_primes >= 1 && num_primes <= 5);
  static constexpr int primes[num_primes] = {Primes...};

  unsigned int pref[num_primes][num_primes] = {};
  unsigned int inv_pref_mont[num_primes] = {};
  Montgomery32 monts[num_primes];
  Barrett32 barretts[num_primes];

  constexpr CrtInfo()
      : monts{Montgomery32((unsigned int)Primes)...},
        barretts{Barrett32((unsigned int)Primes)...} {
    for (int j = 0; j < num_primes; ++j) {
      unsigned int pj = (unsigned int)primes[j];
      pref[0][j] = 1U;
      for (int t = 1; t <= j; ++t) {
        pref[t][j] = (unsigned int)((unsigned long long)pref[t - 1][j] *
                                    (unsigned int)primes[t - 1] % pj);
      }
      if (j >= 1) {
        inv_pref_mont[j] =
            monts[j].pow(monts[j].to_mont(pref[j][j]), primes[j] - 2);
      }
    }
  }
};

template <int... Primes> inline constexpr CrtInfo<Primes...> crt_info{};

// Multi-prime NTT + Garner's CRT reconstruction (1 <= K <= 5 primes < 2^31)
// https://en.wikipedia.org/wiki/Chinese_remainder_theorem
template <int... Primes, typename SrcA, typename SrcB, typename ReduceFinal>
void convolution_garner(SrcA&& get_a, int n, SrcB&& get_b, int k,
                        bool same_input, ReduceFinal&& reduce_final) {
  constexpr int num_primes = sizeof...(Primes);
  const auto& meta = crt_info<Primes...>;
  int s = n + k - 1;
  int z = (int)std::bit_ceil((unsigned int)s);

  NttAlignedBuffer ra(z);
  NttAlignedBuffer rb(same_input ? 0 : z);
  NttAlignedBuffer bufs[num_primes > 1 ? num_primes - 1 : 1];
  for (int j = 0; j < num_primes - 1; ++j) bufs[j] = NttAlignedBuffer(s);

  auto fill_prime = []<int prime, typename Getter>(Getter&& getter, int len,
                                                   unsigned int* dst) {
    using ValT = std::remove_cvref_t<decltype(getter(0))>;
    for (int i = 0; i < len; ++i) {
      if constexpr (is_signed_integer_v<ValT>) {
        if constexpr (sizeof(ValT) <= sizeof(int)) {
          int r = (int)getter(i) % prime;
          if (r < 0) r += prime;
          dst[i] = (unsigned int)r;
        } else {
          long long r = (long long)getter(i) % prime;
          if (r < 0) r += prime;
          dst[i] = (unsigned int)r;
        }
      } else if constexpr (sizeof(ValT) <= sizeof(unsigned int)) {
        dst[i] = (unsigned int)getter(i) % (unsigned int)prime;
      } else {
        dst[i] = (unsigned int)((unsigned long long)getter(i) %
                                (unsigned long long)prime);
      }
    }
  };

  int idx = 0;
  auto run_prime = [&]<int prime>(int p_idx) {
    fill_prime.template operator()<prime>(get_a, n, ra.data);
    ntt_forward_prepared<prime>(ra.data, n, z);
    if (!same_input) {
      fill_prime.template operator()<prime>(get_b, k, rb.data);
      ntt_forward_prepared<prime>(rb.data, k, z);
    }
    unsigned int factor = finish_ntt_convolution<prime>(
        ra.data, same_input ? nullptr : rb.data, z, same_input);
    unsigned int* out = (p_idx == num_primes - 1) ? ra.data : bufs[p_idx].data;
    ntt_scale_out<prime>(ra.data, out, s, factor);
  };
  (run_prime.template operator()<Primes>(idx++), ...);

  for (int i = 0; i < s; ++i) {
    unsigned int x[num_primes] = {};
    x[0] = (num_primes == 1 ? ra.data[i] : bufs[0].data[i]);
    for (int j = 1; j < num_primes; ++j) {
      unsigned int rj = (j == num_primes - 1 ? ra.data[i] : bufs[j].data[i]);
      unsigned long long acc = x[0];
      for (int t = 1; t < j; ++t) {
        acc += (unsigned long long)meta.pref[t][j] * x[t];
      }
      unsigned int cur = meta.barretts[j].reduce(acc);
      unsigned int pj = (unsigned int)meta.primes[j];
      unsigned int diff = rj >= cur ? rj - cur : rj + pj - cur;
      x[j] = meta.monts[j].mul(diff, meta.inv_pref_mont[j]);
    }
    reduce_final(i, x);
  }
}

namespace internal_crt {

template <typename T>
unsigned int normalize_elem32(T v, const Barrett32& barrett) {
  if constexpr (is_signed_integer_v<T>) {
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
  if constexpr (is_signed_integer_v<T>) {
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

}  // namespace internal_crt

template <int MaxLog, int P0, int P1, int P2, typename OutT, typename InT>
std::vector<OutT> convolution_arbitrary_mod32(const std::vector<InT>& a,
                                              const std::vector<InT>& b,
                                              int mod) {
  assert(mod >= 1);
  if (a.empty() || b.empty()) return {};
  int n = (int)a.size();
  int k = (int)b.size();
  long long s = (long long)n + k - 1;
  assert(s <= (1LL << MaxLog));

  auto to_out = [](unsigned int v) -> OutT {
    if constexpr (is_integer_v<OutT>) {
      return (OutT)v;
    } else {
      return OutT::raw(v);
    }
  };

  if (mod == 1) return std::vector<OutT>((int)s, to_out(0));

  Barrett32 barrett((unsigned int)mod);
  bool same_input = (&a == &b);

  NttAlignedBuffer norm_a(is_integer_v<InT> ? n : 0);
  NttAlignedBuffer norm_b((is_integer_v<InT> && !same_input) ? k : 0);
  if constexpr (is_integer_v<InT>) {
    for (int i = 0; i < n; ++i) {
      norm_a.data[i] = internal_crt::normalize_elem32(a[i], barrett);
    }
    if (!same_input) {
      for (int i = 0; i < k; ++i) {
        norm_b.data[i] = internal_crt::normalize_elem32(b[i], barrett);
      }
    }
  }

  auto get_src_a = [&]() {
    if constexpr (is_integer_v<InT>) {
      return (const unsigned int*)norm_a.data;
    } else {
      return a.data();
    }
  };
  auto get_src_b = [&]() {
    if constexpr (is_integer_v<InT>) {
      return (const unsigned int*)(same_input ? norm_a.data : norm_b.data);
    } else {
      return (same_input ? a.data() : b.data());
    }
  };

  if (std::min(n, k) <= ntt_naive_threshold) {
    return internal_crt::convolution_naive32<OutT>(
        get_src_a(), n, get_src_b(), k, (unsigned int)mod, to_out);
  }

  unsigned int m0_mod = barrett.reduce((unsigned long long)P0);
  unsigned int m01_mod = barrett.reduce((unsigned long long)P0 * P1);

  std::vector<OutT> res((int)s);
  const auto* pa = get_src_a();
  const auto* pb = get_src_b();
  auto read_elem = [](const auto* ptr, int i) -> unsigned int {
    if constexpr (is_integer_v<InT>) {
      return ptr[i];
    } else {
      return (unsigned int)ptr[i].val();
    }
  };
  convolution_garner<P0, P1, P2>(
      [&](int i) { return read_elem(pa, i); }, n,
      [&](int i) { return read_elem(pb, i); }, k, same_input,
      [&](int i, const unsigned int* x) {
        unsigned long long acc = x[0] + (unsigned long long)m0_mod * x[1] +
                                 (unsigned long long)m01_mod * x[2];
        res[i] = to_out(barrett.reduce(acc));
      });
  return res;
}

template <int MaxLog, int P0, int P1, int P2, int P3, int P4, typename T>
std::vector<T> convolution_arbitrary_mod64(const std::vector<T>& a,
                                           const std::vector<T>& b,
                                           long long mod) {
  assert(mod >= 1);
  if (a.empty() || b.empty()) return {};
  int n = (int)a.size();
  int k = (int)b.size();
  long long s = (long long)n + k - 1;
  assert(s <= (1LL << MaxLog));
  if (mod == 1) return std::vector<T>((int)s, (T)0);

  bool same_input = (&a == &b);
  NttAlignedBuffer64 norm_a(n);
  for (int i = 0; i < n; ++i) {
    norm_a.data[i] = internal_crt::normalize_elem64(a[i], mod);
  }
  NttAlignedBuffer64 norm_b(same_input ? 0 : k);
  if (!same_input) {
    for (int i = 0; i < k; ++i) {
      norm_b.data[i] = internal_crt::normalize_elem64(b[i], mod);
    }
  }

  unsigned long long um = (unsigned long long)mod;
  if (std::min(n, k) <= ntt_naive_threshold) {
    return internal_crt::convolution_naive64<T>(
        norm_a.data, n, same_input ? norm_a.data : norm_b.data, k, um);
  }

  constexpr unsigned long long m01_u64 = (unsigned long long)P0 * P1;
  unsigned long long m01_mod = m01_u64 % um;
  unsigned long long m012_mod =
      (unsigned long long)((unsigned __int128)m01_mod * P2 % um);
  unsigned long long m0123_mod =
      (unsigned long long)((unsigned __int128)m012_mod * P3 % um);

  std::vector<T> res((int)s);
  const unsigned long long* pb_ptr = same_input ? norm_a.data : norm_b.data;
  convolution_garner<P0, P1, P2, P3, P4>(
      [&](int i) { return norm_a.data[i]; }, n,
      [&](int i) { return pb_ptr[i]; }, k, same_input,
      [&](int i, const unsigned int* x) {
        unsigned __int128 acc = x[0];
        acc += (unsigned __int128)(unsigned int)P0 * x[1];
        acc += (unsigned __int128)m01_mod * x[2];
        acc += (unsigned __int128)m012_mod * x[3];
        acc += (unsigned __int128)m0123_mod * x[4];
        res[i] = (T)(unsigned long long)(acc % um);
      });
  return res;
}

}  // namespace library3372::internal

#endif  // LIBRARY3372_LIBRARY3372_INTERNAL_MATH_CONVOLUTION_GARNER_HPP_
