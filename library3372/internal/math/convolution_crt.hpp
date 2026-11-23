// Repository: Library3372
// File: library3372/internal/math/convolution_crt.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/internal/math/convolution_crt.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_INTERNAL_MATH_CONVOLUTION_CRT_HPP_
#define LIBRARY3372_LIBRARY3372_INTERNAL_MATH_CONVOLUTION_CRT_HPP_

#include <bit>
#include <type_traits>

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
void convolution_crt(SrcA&& get_a, int n, SrcB&& get_b, int k, bool same_input,
                     ReduceFinal&& reduce_final) {
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

}  // namespace library3372::internal

#endif  // LIBRARY3372_LIBRARY3372_INTERNAL_MATH_CONVOLUTION_CRT_HPP_
