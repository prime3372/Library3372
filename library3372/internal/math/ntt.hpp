// Repository: Library3372
// File: library3372/internal/math/ntt.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/internal/math/ntt.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_INTERNAL_MATH_NTT_HPP_
#define LIBRARY3372_LIBRARY3372_INTERNAL_MATH_NTT_HPP_

#ifdef __AVX2__
#include <immintrin.h>
#endif

#include <bit>
#include <cstring>
#include <new>
#include <type_traits>
#include <utility>

#include "library3372/internal/math/montgomery.hpp"
#include "library3372/math/is_prime.hpp"

namespace library3372::internal {

inline constexpr int ntt_naive_threshold = 60;

constexpr int ntt_primitive_root(int m) {
  if (m == 2) return 1;
  int divs[20] = {};
  divs[0] = 2;
  int cnt = 1;
  int x = (m - 1) / 2;
  while (x % 2 == 0) x /= 2;
  for (int i = 3; (long long)i * i <= x; i += 2) {
    if (x % i == 0) {
      divs[cnt++] = i;
      while (x % i == 0) x /= i;
    }
  }
  if (x > 1) divs[cnt++] = x;
  for (int g = 2;; ++g) {
    bool ok = true;
    for (int i = 0; i < cnt; ++i) {
      long long base = g;
      long long exp = (m - 1) / divs[i];
      long long res = 1;
      while (exp > 0) {
        if (exp & 1) res = res * base % m;
        base = base * base % m;
        exp >>= 1;
      }
      if (res == 1) {
        ok = false;
        break;
      }
    }
    if (ok) return g;
  }
}

template <int m> struct NttInfo {
  static constexpr bool valid = (m >= 2 && is_prime(m));
  static constexpr unsigned int mod = (unsigned int)(valid ? m : 3);
  static constexpr bool lazy = (mod < (1U << 30));
  static constexpr int rank2 = std::countr_zero(mod - 1);
  static constexpr Montgomery32 mont{mod == 2 ? 3U : mod};

  unsigned int imag = 0;
  unsigned int iimag = 0;
  unsigned int step[13][32] = {};
  unsigned int istep[13][32] = {};
  alignas(32) unsigned int w4_init[8] = {};
  alignas(32) unsigned int w2_init[8] = {};
  alignas(32) unsigned int w1_init[8] = {};
  alignas(32) unsigned int iw4_init[8] = {};
  alignas(32) unsigned int iw2_init[8] = {};
  alignas(32) unsigned int iw1_init[8] = {};

  constexpr NttInfo() {
    if constexpr (!valid || mod == 2) return;
    int g = ntt_primitive_root((int)mod);
    unsigned int g_mont = mont.to_mont((unsigned int)g);

    unsigned int root[32] = {};
    unsigned int iroot[32] = {};
    root[rank2] = mont.pow(g_mont, (mod - 1) >> rank2);
    iroot[rank2] = mont.pow(root[rank2], mod - 2);
    for (int i = rank2 - 1; i >= 0; --i) {
      root[i] = mont.mul(root[i + 1], root[i + 1]);
      iroot[i] = mont.mul(iroot[i + 1], iroot[i + 1]);
    }

    if (rank2 >= 2) {
      imag = root[2];
      iimag = iroot[2];
    }

    for (int d = 0; d < 13; ++d) {
      unsigned int prod = mont.one();
      unsigned int iprod = mont.one();
      for (int k = 0; k + d <= rank2 - 2; ++k) {
        step[d][k] = mont.mul(root[k + d + 2], prod);
        istep[d][k] = mont.mul(iroot[k + d + 2], iprod);
        prod = mont.mul(prod, iroot[k + d + 2]);
        iprod = mont.mul(iprod, root[k + d + 2]);
      }
    }

    if (rank2 >= 4) {
      unsigned int e0 = root[2];
      unsigned int e1 = root[3];
      unsigned int e2 = root[4];
      unsigned int e01 = mont.mul(e0, e1);
      unsigned int e02 = mont.mul(e0, e2);
      unsigned int e12 = mont.mul(e1, e2);
      unsigned int e012 = mont.mul(e01, e2);

      for (int i = 0; i < 4; ++i) w4_init[i] = mont.one();
      for (int i = 4; i < 8; ++i) w4_init[i] = e0;

      w2_init[0] = mont.one();
      w2_init[1] = mont.one();
      w2_init[2] = e0;
      w2_init[3] = e0;
      w2_init[4] = e1;
      w2_init[5] = e1;
      w2_init[6] = e01;
      w2_init[7] = e01;

      w1_init[0] = mont.one();
      w1_init[1] = e1;
      w1_init[2] = e0;
      w1_init[3] = e01;
      w1_init[4] = e2;
      w1_init[5] = e12;
      w1_init[6] = e02;
      w1_init[7] = e012;

      unsigned int ie0 = iroot[2];
      unsigned int ie1 = iroot[3];
      unsigned int ie2 = iroot[4];
      unsigned int ie01 = mont.mul(ie0, ie1);
      unsigned int ie02 = mont.mul(ie0, ie2);
      unsigned int ie12 = mont.mul(ie1, ie2);
      unsigned int ie012 = mont.mul(ie01, ie2);

      for (int i = 0; i < 4; ++i) iw4_init[i] = mont.one();
      for (int i = 4; i < 8; ++i) iw4_init[i] = ie0;

      iw2_init[0] = mont.one();
      iw2_init[1] = mont.one();
      iw2_init[2] = ie0;
      iw2_init[3] = ie0;
      iw2_init[4] = ie1;
      iw2_init[5] = ie1;
      iw2_init[6] = ie01;
      iw2_init[7] = ie01;

      iw1_init[0] = mont.one();
      iw1_init[1] = ie1;
      iw1_init[2] = ie0;
      iw1_init[3] = ie01;
      iw1_init[4] = ie2;
      iw1_init[5] = ie12;
      iw1_init[6] = ie02;
      iw1_init[7] = ie012;
    }
  }
};

template <int m> inline constexpr NttInfo<m> ntt_info{};

struct NttAlignedBuffer {
  unsigned int* data = nullptr;

  NttAlignedBuffer() = default;
  explicit NttAlignedBuffer(int n)
      : data(n > 0 ? new (std::align_val_t(32)) unsigned int[n] : nullptr) {}

  ~NttAlignedBuffer() {
    if (data != nullptr) ::operator delete[](data, std::align_val_t(32));
  }

  NttAlignedBuffer(const NttAlignedBuffer&) = delete;
  NttAlignedBuffer& operator=(const NttAlignedBuffer&) = delete;

  NttAlignedBuffer(NttAlignedBuffer&& other) noexcept : data(other.data) {
    other.data = nullptr;
  }

  NttAlignedBuffer& operator=(NttAlignedBuffer&& other) noexcept {
    if (this != &other) {
      if (data != nullptr) ::operator delete[](data, std::align_val_t(32));
      data = other.data;
      other.data = nullptr;
    }
    return *this;
  }
};

namespace internal_ntt {

template <bool Lazy>
inline unsigned int add_lim_scalar(unsigned int a, unsigned int b,
                                   unsigned int lim) {
  unsigned int s = a + b;
  return s >= lim ? s - lim : s;
}

template <bool Lazy>
inline unsigned int sub_lim_scalar(unsigned int a, unsigned int b,
                                   unsigned int lim) {
  return a >= b ? a - b : a + lim - b;
}

template <bool Lazy>
inline unsigned int mont_mul_scalar(unsigned int a, unsigned int b,
                                    const Montgomery32& mont) {
  if constexpr (Lazy) return mont.mul_2m(a, b);
  return mont.mul(a, b);
}

#ifdef __AVX2__

struct Avx2MontConsts {
  __m256i mod;
  __m256i lim;
  __m256i inv;

  static Avx2MontConsts create(unsigned int m, unsigned int limit) {
    return {
        _mm256_set1_epi32((int)m),
        _mm256_set1_epi32((int)limit),
        _mm256_set1_epi32((int)Montgomery32(m).neg_inv()),
    };
  }
};

inline __m256i add_lim_avx2(__m256i a, __m256i b, __m256i lim) {
  __m256i s = _mm256_add_epi32(a, b);
  return _mm256_min_epu32(s, _mm256_sub_epi32(s, lim));
}

inline __m256i sub_lim_avx2(__m256i a, __m256i b, __m256i lim) {
  __m256i d = _mm256_sub_epi32(a, b);
  return _mm256_min_epu32(d, _mm256_add_epi32(d, lim));
}

template <bool Lazy>
inline __m256i mont_mul_avx2(__m256i a, __m256i b, const Avx2MontConsts& c) {
  __m256i a_odd = _mm256_srli_epi64(a, 32);
  __m256i b_odd = _mm256_srli_epi64(b, 32);
  __m256i t_e = _mm256_mul_epu32(a, b);
  __m256i t_o = _mm256_mul_epu32(a_odd, b_odd);
  __m256i q_e = _mm256_mul_epu32(t_e, c.inv);
  __m256i q_o = _mm256_mul_epu32(t_o, c.inv);
  __m256i qm_e = _mm256_mul_epu32(q_e, c.mod);
  __m256i qm_o = _mm256_mul_epu32(q_o, c.mod);
  __m256i s_e = _mm256_add_epi64(t_e, qm_e);
  __m256i s_o = _mm256_add_epi64(t_o, qm_o);
  __m256i u = _mm256_blend_epi32(_mm256_srli_epi64(s_e, 32), s_o, 0xAA);
  if constexpr (!Lazy) {
    u = _mm256_min_epu32(u, _mm256_sub_epi32(u, c.mod));
  }
  return u;
}

template <bool Lazy>
inline __m256i mont_mul_bcast_avx2(__m256i a, __m256i b_bcast,
                                   const Avx2MontConsts& c) {
  __m256i a_odd = _mm256_srli_epi64(a, 32);
  __m256i t_e = _mm256_mul_epu32(a, b_bcast);
  __m256i t_o = _mm256_mul_epu32(a_odd, b_bcast);
  __m256i q_e = _mm256_mul_epu32(t_e, c.inv);
  __m256i q_o = _mm256_mul_epu32(t_o, c.inv);
  __m256i qm_e = _mm256_mul_epu32(q_e, c.mod);
  __m256i qm_o = _mm256_mul_epu32(q_o, c.mod);
  __m256i s_e = _mm256_add_epi64(t_e, qm_e);
  __m256i s_o = _mm256_add_epi64(t_o, qm_o);
  __m256i u = _mm256_blend_epi32(_mm256_srli_epi64(s_e, 32), s_o, 0xAA);
  if constexpr (!Lazy) {
    u = _mm256_min_epu32(u, _mm256_sub_epi32(u, c.mod));
  }
  return u;
}

template <int m, int FixedQ = 0>
inline void radix4_stage_fwd(unsigned int* a, int n, int h, unsigned int w1,
                             const Avx2MontConsts& c, __m256i vimag) {
  const auto& info = ntt_info<m>;
  constexpr bool lazy = NttInfo<m>::lazy;
  int q = FixedQ > 0 ? FixedQ : (1 << (h - 2));
  int blocks = FixedQ > 0 ? (n / (FixedQ << 2)) : (n >> h);
  for (int s = 0; s < blocks; ++s) {
    unsigned int w2 = mont_mul_scalar<lazy>(w1, w1, info.mont);
    unsigned int w3 = mont_mul_scalar<lazy>(w1, w2, info.mont);
    __m256i vw1 = _mm256_set1_epi32((int)w1);
    __m256i vw2 = _mm256_set1_epi32((int)w2);
    __m256i vw3 = _mm256_set1_epi32((int)w3);
    unsigned int* p0 = a + (FixedQ > 0 ? s * (FixedQ << 2) : (s << h));
    unsigned int* p1 = p0 + q;
    unsigned int* p2 = p1 + q;
    unsigned int* p3 = p2 + q;
    for (int i = 0; i < q; i += 8) {
      __m256i a0 = _mm256_load_si256((const __m256i*)(p0 + i));
      __m256i a1 = _mm256_load_si256((const __m256i*)(p1 + i));
      __m256i a2 = _mm256_load_si256((const __m256i*)(p2 + i));
      __m256i a3 = _mm256_load_si256((const __m256i*)(p3 + i));

      __m256i x1 = mont_mul_bcast_avx2<lazy>(a1, vw1, c);
      __m256i x2 = mont_mul_bcast_avx2<lazy>(a2, vw2, c);
      __m256i x3 = mont_mul_bcast_avx2<lazy>(a3, vw3, c);

      __m256i u0 = add_lim_avx2(a0, x2, c.lim);
      __m256i u2 = sub_lim_avx2(a0, x2, c.lim);
      __m256i u1 = add_lim_avx2(x1, x3, c.lim);
      __m256i diff13 = _mm256_sub_epi32(_mm256_add_epi32(x1, c.lim), x3);
      __m256i u3 = mont_mul_bcast_avx2<lazy>(diff13, vimag, c);

      _mm256_store_si256((__m256i*)(p0 + i), add_lim_avx2(u0, u1, c.lim));
      _mm256_store_si256((__m256i*)(p1 + i), sub_lim_avx2(u0, u1, c.lim));
      _mm256_store_si256((__m256i*)(p2 + i), add_lim_avx2(u2, u3, c.lim));
      _mm256_store_si256((__m256i*)(p3 + i), sub_lim_avx2(u2, u3, c.lim));
    }
    if (s + 1 < blocks) {
      w1 = mont_mul_scalar<lazy>(
          w1, info.step[1][std::countr_one((unsigned int)s)], info.mont);
    }
  }
}

template <int m>
inline void last3_fwd_pair(unsigned int* ptr, __m256i w4, __m256i w2,
                           __m256i w1, const Avx2MontConsts& c) {
  constexpr bool lazy = NttInfo<m>::lazy;
  __m256i v0 = _mm256_load_si256((const __m256i*)ptr);
  __m256i v1 = _mm256_load_si256((const __m256i*)(ptr + 8));

  __m256i lo = _mm256_permute2x128_si256(v0, v1, 0x20);
  __m256i hi = _mm256_permute2x128_si256(v0, v1, 0x31);

  __m256i r4 = mont_mul_avx2<lazy>(hi, w4, c);
  __m256i a4 = add_lim_avx2(lo, r4, c.lim);
  __m256i b4 = sub_lim_avx2(lo, r4, c.lim);

  __m256i x0 = _mm256_unpacklo_epi64(a4, b4);
  __m256i x1 = _mm256_unpackhi_epi64(a4, b4);

  __m256i r2 = mont_mul_avx2<lazy>(x1, w2, c);
  __m256i a2 = add_lim_avx2(x0, r2, c.lim);
  __m256i b2 = sub_lim_avx2(x0, r2, c.lim);

  __m256i y0 = _mm256_castps_si256(_mm256_shuffle_ps(
      _mm256_castsi256_ps(a2), _mm256_castsi256_ps(b2), 0x88));
  __m256i y1 = _mm256_castps_si256(_mm256_shuffle_ps(
      _mm256_castsi256_ps(a2), _mm256_castsi256_ps(b2), 0xDD));

  __m256i r1 = mont_mul_avx2<lazy>(y1, w1, c);
  __m256i a1 = add_lim_avx2(y0, r1, c.lim);
  __m256i b1 = sub_lim_avx2(y0, r1, c.lim);

  __m256i z0 = _mm256_unpacklo_epi32(a1, b1);
  __m256i z1 = _mm256_unpackhi_epi32(a1, b1);
  __m256i lo_out = _mm256_unpacklo_epi64(z0, z1);
  __m256i hi_out = _mm256_unpackhi_epi64(z0, z1);
  _mm256_store_si256((__m256i*)ptr,
                     _mm256_permute2x128_si256(lo_out, hi_out, 0x20));
  _mm256_store_si256((__m256i*)(ptr + 8),
                     _mm256_permute2x128_si256(lo_out, hi_out, 0x31));
}

template <int m>
inline void last3_fwd(unsigned int* a, int t_end, __m256i w4_0, __m256i w2_0,
                      __m256i w1_0, const Avx2MontConsts& c) {
  if (t_end == 1) {
    last3_fwd_pair<m>(a, w4_0, w2_0, w1_0, c);
    return;
  }
  const auto& info = ntt_info<m>;
  constexpr bool lazy = NttInfo<m>::lazy;
  __m256i w4_1 = mont_mul_bcast_avx2<lazy>(
      w4_0, _mm256_set1_epi32((int)info.step[1][0]), c);
  __m256i w2_1 = mont_mul_bcast_avx2<lazy>(
      w2_0, _mm256_set1_epi32((int)info.step[2][0]), c);
  __m256i w1_1 = mont_mul_bcast_avx2<lazy>(
      w1_0, _mm256_set1_epi32((int)info.step[3][0]), c);
  int u_end = t_end >> 1;
  for (int u = 0; u < u_end; ++u) {
    unsigned int* ptr = a + (u << 5);
    last3_fwd_pair<m>(ptr, w4_0, w2_0, w1_0, c);
    last3_fwd_pair<m>(ptr + 16, w4_1, w2_1, w1_1, c);
    if (u + 1 < u_end) {
      int k = std::countr_one((unsigned int)u);
      __m256i s4 = _mm256_set1_epi32((int)info.step[2][k]);
      __m256i s2 = _mm256_set1_epi32((int)info.step[3][k]);
      __m256i s1 = _mm256_set1_epi32((int)info.step[4][k]);
      w4_0 = mont_mul_bcast_avx2<lazy>(w4_0, s4, c);
      w4_1 = mont_mul_bcast_avx2<lazy>(w4_1, s4, c);
      w2_0 = mont_mul_bcast_avx2<lazy>(w2_0, s2, c);
      w2_1 = mont_mul_bcast_avx2<lazy>(w2_1, s2, c);
      w1_0 = mont_mul_bcast_avx2<lazy>(w1_0, s1, c);
      w1_1 = mont_mul_bcast_avx2<lazy>(w1_1, s1, c);
    }
  }
}

template <int m, int FixedQ = 0>
inline void radix4_stage_inv(unsigned int* a, int n, int h, unsigned int iw1_s,
                             const Avx2MontConsts& c, __m256i viimag) {
  const auto& info = ntt_info<m>;
  constexpr bool lazy = NttInfo<m>::lazy;
  int q = FixedQ > 0 ? FixedQ : (1 << (h - 2));
  int blocks = FixedQ > 0 ? (n / (FixedQ << 2)) : (n >> h);
  for (int s = 0; s < blocks; ++s) {
    unsigned int iw2_s = mont_mul_scalar<lazy>(iw1_s, iw1_s, info.mont);
    unsigned int iw3_s = mont_mul_scalar<lazy>(iw1_s, iw2_s, info.mont);
    __m256i vw1 = _mm256_set1_epi32((int)iw1_s);
    __m256i vw2 = _mm256_set1_epi32((int)iw2_s);
    __m256i vw3 = _mm256_set1_epi32((int)iw3_s);
    unsigned int* p0 = a + (FixedQ > 0 ? s * (FixedQ << 2) : (s << h));
    unsigned int* p1 = p0 + q;
    unsigned int* p2 = p1 + q;
    unsigned int* p3 = p2 + q;
    for (int i = 0; i < q; i += 8) {
      __m256i c0 = _mm256_load_si256((const __m256i*)(p0 + i));
      __m256i c1 = _mm256_load_si256((const __m256i*)(p1 + i));
      __m256i c2 = _mm256_load_si256((const __m256i*)(p2 + i));
      __m256i c3 = _mm256_load_si256((const __m256i*)(p3 + i));

      __m256i u0 = add_lim_avx2(c0, c1, c.lim);
      __m256i u1 = sub_lim_avx2(c0, c1, c.lim);
      __m256i u2 = add_lim_avx2(c2, c3, c.lim);
      __m256i d23 = _mm256_sub_epi32(_mm256_add_epi32(c2, c.lim), c3);
      __m256i u3 = mont_mul_bcast_avx2<lazy>(d23, viimag, c);

      __m256i x0 = add_lim_avx2(u0, u2, c.lim);
      __m256i d02 = _mm256_sub_epi32(_mm256_add_epi32(u0, c.lim), u2);
      __m256i x2 = mont_mul_bcast_avx2<lazy>(d02, vw2, c);
      __m256i x1 = mont_mul_bcast_avx2<lazy>(_mm256_add_epi32(u1, u3), vw1, c);
      __m256i d13 = _mm256_sub_epi32(_mm256_add_epi32(u1, c.lim), u3);
      __m256i x3 = mont_mul_bcast_avx2<lazy>(d13, vw3, c);

      _mm256_store_si256((__m256i*)(p0 + i), x0);
      _mm256_store_si256((__m256i*)(p1 + i), x1);
      _mm256_store_si256((__m256i*)(p2 + i), x2);
      _mm256_store_si256((__m256i*)(p3 + i), x3);
    }
    if (s + 1 < blocks) {
      iw1_s = mont_mul_scalar<lazy>(
          iw1_s, info.istep[1][std::countr_one((unsigned int)s)], info.mont);
    }
  }
}

template <int m>
inline void last3_inv_pair(unsigned int* ptr, __m256i iw4, __m256i iw2,
                           __m256i iw1, const Avx2MontConsts& c) {
  constexpr bool lazy = NttInfo<m>::lazy;
  __m256i v0 = _mm256_load_si256((const __m256i*)ptr);
  __m256i v1 = _mm256_load_si256((const __m256i*)(ptr + 8));

  __m256i lo = _mm256_permute2x128_si256(v0, v1, 0x20);
  __m256i hi = _mm256_permute2x128_si256(v0, v1, 0x31);
  __m256i x0 = _mm256_unpacklo_epi64(lo, hi);
  __m256i x1 = _mm256_unpackhi_epi64(lo, hi);
  __m256i y0 = _mm256_castps_si256(_mm256_shuffle_ps(
      _mm256_castsi256_ps(x0), _mm256_castsi256_ps(x1), 0x88));
  __m256i y1 = _mm256_castps_si256(_mm256_shuffle_ps(
      _mm256_castsi256_ps(x0), _mm256_castsi256_ps(x1), 0xDD));

  __m256i a1 = add_lim_avx2(y0, y1, c.lim);
  __m256i d1 = _mm256_sub_epi32(_mm256_add_epi32(y0, c.lim), y1);
  __m256i b1 = mont_mul_avx2<lazy>(d1, iw1, c);

  __m256i z0 = _mm256_unpacklo_epi32(a1, b1);
  __m256i z1 = _mm256_unpackhi_epi32(a1, b1);

  __m256i a2 = add_lim_avx2(z0, z1, c.lim);
  __m256i d2 = _mm256_sub_epi32(_mm256_add_epi32(z0, c.lim), z1);
  __m256i b2 = mont_mul_avx2<lazy>(d2, iw2, c);

  __m256i lo4 = _mm256_unpacklo_epi64(a2, b2);
  __m256i hi4 = _mm256_unpackhi_epi64(a2, b2);

  __m256i a4 = add_lim_avx2(lo4, hi4, c.lim);
  __m256i d4 = _mm256_sub_epi32(_mm256_add_epi32(lo4, c.lim), hi4);
  __m256i b4 = mont_mul_avx2<lazy>(d4, iw4, c);

  _mm256_store_si256((__m256i*)ptr, _mm256_permute2x128_si256(a4, b4, 0x20));
  _mm256_store_si256((__m256i*)(ptr + 8),
                     _mm256_permute2x128_si256(a4, b4, 0x31));
}

template <int m>
inline void last3_inv(unsigned int* a, int t_end, __m256i iw4_0, __m256i iw2_0,
                      __m256i iw1_0, const Avx2MontConsts& c) {
  if (t_end == 1) {
    last3_inv_pair<m>(a, iw4_0, iw2_0, iw1_0, c);
    return;
  }
  const auto& info = ntt_info<m>;
  constexpr bool lazy = NttInfo<m>::lazy;
  __m256i iw4_1 = mont_mul_bcast_avx2<lazy>(
      iw4_0, _mm256_set1_epi32((int)info.istep[1][0]), c);
  __m256i iw2_1 = mont_mul_bcast_avx2<lazy>(
      iw2_0, _mm256_set1_epi32((int)info.istep[2][0]), c);
  __m256i iw1_1 = mont_mul_bcast_avx2<lazy>(
      iw1_0, _mm256_set1_epi32((int)info.istep[3][0]), c);
  int u_end = t_end >> 1;
  for (int u = 0; u < u_end; ++u) {
    unsigned int* ptr = a + (u << 5);
    last3_inv_pair<m>(ptr, iw4_0, iw2_0, iw1_0, c);
    last3_inv_pair<m>(ptr + 16, iw4_1, iw2_1, iw1_1, c);
    if (u + 1 < u_end) {
      int k = std::countr_one((unsigned int)u);
      __m256i s4 = _mm256_set1_epi32((int)info.istep[2][k]);
      __m256i s2 = _mm256_set1_epi32((int)info.istep[3][k]);
      __m256i s1 = _mm256_set1_epi32((int)info.istep[4][k]);
      iw4_0 = mont_mul_bcast_avx2<lazy>(iw4_0, s4, c);
      iw4_1 = mont_mul_bcast_avx2<lazy>(iw4_1, s4, c);
      iw2_0 = mont_mul_bcast_avx2<lazy>(iw2_0, s2, c);
      iw2_1 = mont_mul_bcast_avx2<lazy>(iw2_1, s2, c);
      iw1_0 = mont_mul_bcast_avx2<lazy>(iw1_0, s1, c);
      iw1_1 = mont_mul_bcast_avx2<lazy>(iw1_1, s1, c);
    }
  }
}

template <int m, bool HalfZero = false> void butterfly(unsigned int* a, int n) {
  const auto& info = ntt_info<m>;
  constexpr unsigned int mod = NttInfo<m>::mod;
  constexpr bool lazy = NttInfo<m>::lazy;
  constexpr unsigned int lim = lazy ? mod * 2U : mod;
  const Avx2MontConsts c = Avx2MontConsts::create(mod, lim);
  int log_n = std::countr_zero((unsigned int)n);

  if (n < 16) {
    int start_len = n >> 1;
    if constexpr (HalfZero) {
      std::memcpy(a + start_len, a,
                  sizeof(unsigned int) * (unsigned int)start_len);
      start_len >>= 1;
    }
    for (int len = start_len; len >= 1; len >>= 1) {
      int blocks = n / (len << 1);
      unsigned int rot = info.mont.one();
      for (int s = 0; s < blocks; ++s) {
        int off = s * (len << 1);
        for (int i = 0; i < len; ++i) {
          unsigned int l = a[off + i];
          unsigned int r = info.mont.mul(a[off + i + len], rot);
          unsigned int sum = l + r;
          a[off + i] = sum >= mod ? sum - mod : sum;
          a[off + i + len] = l >= r ? l - r : l + mod - r;
        }
        if (s + 1 < blocks) {
          rot = info.mont.mul(rot,
                              info.step[0][std::countr_one((unsigned int)s)]);
        }
      }
    }
    return;
  }

  const __m256i vimag = _mm256_set1_epi32((int)info.imag);
  int h = log_n;
  if ((h - 3) & 1) {
    int p = n >> 1;
    if constexpr (HalfZero) {
      std::memcpy(a + p, a, sizeof(unsigned int) * (unsigned int)p);
    } else {
      for (int i = 0; i < p; i += 8) {
        __m256i x = _mm256_load_si256((const __m256i*)(a + i));
        __m256i y = _mm256_load_si256((const __m256i*)(a + i + p));
        _mm256_store_si256((__m256i*)(a + i), add_lim_avx2(x, y, c.lim));
        _mm256_store_si256((__m256i*)(a + i + p), sub_lim_avx2(x, y, c.lim));
      }
    }
    --h;
  } else if constexpr (HalfZero) {
    int q = n >> 2;
    unsigned int* p0 = a;
    unsigned int* p1 = a + q;
    unsigned int* p2 = p1 + q;
    unsigned int* p3 = p2 + q;
    for (int i = 0; i < q; i += 8) {
      __m256i a0 = _mm256_load_si256((const __m256i*)(p0 + i));
      __m256i a1 = _mm256_load_si256((const __m256i*)(p1 + i));
      __m256i u3 = mont_mul_bcast_avx2<lazy>(a1, vimag, c);
      _mm256_store_si256((__m256i*)(p0 + i), add_lim_avx2(a0, a1, c.lim));
      _mm256_store_si256((__m256i*)(p1 + i), sub_lim_avx2(a0, a1, c.lim));
      _mm256_store_si256((__m256i*)(p2 + i), add_lim_avx2(a0, u3, c.lim));
      _mm256_store_si256((__m256i*)(p3 + i), sub_lim_avx2(a0, u3, c.lim));
    }
    h -= 2;
  }

  __m256i w4 = _mm256_load_si256((const __m256i*)info.w4_init);
  __m256i w2 = _mm256_load_si256((const __m256i*)info.w2_init);
  __m256i w1 = _mm256_load_si256((const __m256i*)info.w1_init);

  if (h <= 13) {
    for (; h > 5; h -= 2) {
      radix4_stage_fwd<m>(a, n, h, info.mont.one(), c, vimag);
    }
    if (h == 5) radix4_stage_fwd<m, 8>(a, n, 5, info.mont.one(), c, vimag);
    last3_fwd<m>(a, n >> 4, w4, w2, w1, c);
    return;
  }

  for (; h > 13; h -= 2) {
    radix4_stage_fwd<m>(a, n, h, info.mont.one(), c, vimag);
  }

  unsigned int w_init[5] = {info.mont.one(), info.mont.one(), info.mont.one(),
                            info.mont.one(), info.mont.one()};
  int s_end = n >> 13;
  for (int s = 0; s < s_end; ++s) {
    unsigned int* blk = a + (s << 13);
    radix4_stage_fwd<m>(blk, 1 << 13, 13, w_init[0], c, vimag);
    radix4_stage_fwd<m>(blk, 1 << 13, 11, w_init[1], c, vimag);
    radix4_stage_fwd<m>(blk, 1 << 13, 9, w_init[2], c, vimag);
    radix4_stage_fwd<m>(blk, 1 << 13, 7, w_init[3], c, vimag);
    radix4_stage_fwd<m, 8>(blk, 1 << 13, 5, w_init[4], c, vimag);
    last3_fwd<m>(blk, 1 << 9, w4, w2, w1, c);
    if (s + 1 < s_end) {
      int k = std::countr_one((unsigned int)s);
      w_init[0] = mont_mul_scalar<lazy>(w_init[0], info.step[1][k], info.mont);
      w_init[1] = mont_mul_scalar<lazy>(w_init[1], info.step[3][k], info.mont);
      w_init[2] = mont_mul_scalar<lazy>(w_init[2], info.step[5][k], info.mont);
      w_init[3] = mont_mul_scalar<lazy>(w_init[3], info.step[7][k], info.mont);
      w_init[4] = mont_mul_scalar<lazy>(w_init[4], info.step[9][k], info.mont);
      w4 = mont_mul_bcast_avx2<lazy>(
          w4, _mm256_set1_epi32((int)info.step[10][k]), c);
      w2 = mont_mul_bcast_avx2<lazy>(
          w2, _mm256_set1_epi32((int)info.step[11][k]), c);
      w1 = mont_mul_bcast_avx2<lazy>(
          w1, _mm256_set1_epi32((int)info.step[12][k]), c);
    }
  }
}

template <int m> void butterfly_inv(unsigned int* a, int n) {
  const auto& info = ntt_info<m>;
  constexpr unsigned int mod = NttInfo<m>::mod;
  constexpr bool lazy = NttInfo<m>::lazy;
  constexpr unsigned int lim = lazy ? mod * 2U : mod;
  const Avx2MontConsts c = Avx2MontConsts::create(mod, lim);
  int log_n = std::countr_zero((unsigned int)n);

  if (n < 16) {
    for (int len = 1; len < n; len <<= 1) {
      int blocks = n / (len << 1);
      unsigned int irot = info.mont.one();
      for (int s = 0; s < blocks; ++s) {
        int off = s * (len << 1);
        for (int i = 0; i < len; ++i) {
          unsigned int l = a[off + i];
          unsigned int r = a[off + i + len];
          unsigned int sum = l + r;
          a[off + i] = sum >= mod ? sum - mod : sum;
          unsigned int diff = l >= r ? l - r : l + mod - r;
          a[off + i + len] = info.mont.mul(diff, irot);
        }
        if (s + 1 < blocks) {
          irot = info.mont.mul(irot,
                               info.istep[0][std::countr_one((unsigned int)s)]);
        }
      }
    }
    return;
  }

  const __m256i viimag = _mm256_set1_epi32((int)info.iimag);
  __m256i iw4 = _mm256_load_si256((const __m256i*)info.iw4_init);
  __m256i iw2 = _mm256_load_si256((const __m256i*)info.iw2_init);
  __m256i iw1 = _mm256_load_si256((const __m256i*)info.iw1_init);

  if (log_n <= 13) {
    last3_inv<m>(a, n >> 4, iw4, iw2, iw1, c);
    int h = 5;
    if (h <= log_n) {
      radix4_stage_inv<m, 8>(a, n, 5, info.mont.one(), c, viimag);
      h = 7;
    }
    for (; h <= log_n; h += 2) {
      radix4_stage_inv<m>(a, n, h, info.mont.one(), c, viimag);
    }
    if (h - 1 == log_n) {
      int p = n >> 1;
      for (int i = 0; i < p; i += 8) {
        __m256i x = _mm256_load_si256((const __m256i*)(a + i));
        __m256i y = _mm256_load_si256((const __m256i*)(a + i + p));
        _mm256_store_si256((__m256i*)(a + i), add_lim_avx2(x, y, c.lim));
        _mm256_store_si256((__m256i*)(a + i + p), sub_lim_avx2(x, y, c.lim));
      }
    }
    return;
  }

  unsigned int iw_init[5] = {info.mont.one(), info.mont.one(), info.mont.one(),
                             info.mont.one(), info.mont.one()};
  int s_end = n >> 13;
  for (int s = 0; s < s_end; ++s) {
    unsigned int* blk = a + (s << 13);
    last3_inv<m>(blk, 1 << 9, iw4, iw2, iw1, c);
    radix4_stage_inv<m, 8>(blk, 1 << 13, 5, iw_init[4], c, viimag);
    radix4_stage_inv<m>(blk, 1 << 13, 7, iw_init[3], c, viimag);
    radix4_stage_inv<m>(blk, 1 << 13, 9, iw_init[2], c, viimag);
    radix4_stage_inv<m>(blk, 1 << 13, 11, iw_init[1], c, viimag);
    radix4_stage_inv<m>(blk, 1 << 13, 13, iw_init[0], c, viimag);
    if (s + 1 < s_end) {
      int k = std::countr_one((unsigned int)s);
      iw_init[0] =
          mont_mul_scalar<lazy>(iw_init[0], info.istep[1][k], info.mont);
      iw_init[1] =
          mont_mul_scalar<lazy>(iw_init[1], info.istep[3][k], info.mont);
      iw_init[2] =
          mont_mul_scalar<lazy>(iw_init[2], info.istep[5][k], info.mont);
      iw_init[3] =
          mont_mul_scalar<lazy>(iw_init[3], info.istep[7][k], info.mont);
      iw_init[4] =
          mont_mul_scalar<lazy>(iw_init[4], info.istep[9][k], info.mont);
      iw4 = mont_mul_bcast_avx2<lazy>(
          iw4, _mm256_set1_epi32((int)info.istep[10][k]), c);
      iw2 = mont_mul_bcast_avx2<lazy>(
          iw2, _mm256_set1_epi32((int)info.istep[11][k]), c);
      iw1 = mont_mul_bcast_avx2<lazy>(
          iw1, _mm256_set1_epi32((int)info.istep[12][k]), c);
    }
  }

  int h = 15;
  for (; h <= log_n; h += 2) {
    radix4_stage_inv<m>(a, n, h, info.mont.one(), c, viimag);
  }
  if (h - 1 == log_n) {
    int p = n >> 1;
    for (int i = 0; i < p; i += 8) {
      __m256i x = _mm256_load_si256((const __m256i*)(a + i));
      __m256i y = _mm256_load_si256((const __m256i*)(a + i + p));
      _mm256_store_si256((__m256i*)(a + i), add_lim_avx2(x, y, c.lim));
      _mm256_store_si256((__m256i*)(a + i + p), sub_lim_avx2(x, y, c.lim));
    }
  }
}

template <int m>
void pointwise_mul(unsigned int* a, const unsigned int* b, int n) {
  constexpr unsigned int mod = NttInfo<m>::mod;
  constexpr bool lazy = NttInfo<m>::lazy;
  const Avx2MontConsts c = Avx2MontConsts::create(mod, lazy ? mod * 2U : mod);
  int i = 0;
  for (; i + 8 <= n; i += 8) {
    __m256i va = _mm256_load_si256((const __m256i*)(a + i));
    __m256i vb = _mm256_load_si256((const __m256i*)(b + i));
    _mm256_store_si256((__m256i*)(a + i), mont_mul_avx2<lazy>(va, vb, c));
  }
  for (; i < n; ++i) {
    a[i] = mont_mul_scalar<lazy>(a[i], b[i], ntt_info<m>.mont);
  }
}

template <int m, typename OutPtr, typename Transform>
void scale_out_impl(const unsigned int* src, OutPtr dst, int n,
                    unsigned int factor, Transform&& transform) {
  constexpr unsigned int mod = NttInfo<m>::mod;
  const Avx2MontConsts c = Avx2MontConsts::create(mod, mod);
  __m256i vf = _mm256_set1_epi32((int)factor);
  int i = 0;
  for (; i + 8 <= n; i += 8) {
    __m256i va = _mm256_load_si256((const __m256i*)(src + i));
    __m256i vr = mont_mul_bcast_avx2<false>(va, vf, c);
    if constexpr (std::is_same_v<OutPtr, unsigned int*>) {
      _mm256_storeu_si256((__m256i*)(dst + i), vr);
    } else {
      alignas(32) unsigned int tmp[8];
      _mm256_store_si256((__m256i*)tmp, vr);
      for (int j = 0; j < 8; ++j) dst[i + j] = transform(tmp[j]);
    }
  }
  for (; i < n; ++i) dst[i] = transform(ntt_info<m>.mont.mul(src[i], factor));
}

#else

template <int m, int FixedQ = 0>
inline void radix4_stage_scalar_fwd(unsigned int* a, int n, int h,
                                    unsigned int w1) {
  const auto& info = ntt_info<m>;
  constexpr bool lazy = NttInfo<m>::lazy;
  constexpr unsigned int lim = lazy ? NttInfo<m>::mod * 2U : NttInfo<m>::mod;
  if constexpr (FixedQ == 1) {
    int blocks = n >> 2;
    auto bfly4 = [&](unsigned int* p, unsigned int w1_s) {
      unsigned int w2_s = mont_mul_scalar<lazy>(w1_s, w1_s, info.mont);
      unsigned int w3_s = mont_mul_scalar<lazy>(w1_s, w2_s, info.mont);
      unsigned int a0 = p[0];
      unsigned int x1 = mont_mul_scalar<lazy>(p[1], w1_s, info.mont);
      unsigned int x2 = mont_mul_scalar<lazy>(p[2], w2_s, info.mont);
      unsigned int x3 = mont_mul_scalar<lazy>(p[3], w3_s, info.mont);
      unsigned int u0 = add_lim_scalar<lazy>(a0, x2, lim);
      unsigned int u2 = sub_lim_scalar<lazy>(a0, x2, lim);
      unsigned int u1 = add_lim_scalar<lazy>(x1, x3, lim);
      unsigned int u3 =
          mont_mul_scalar<lazy>(x1 + lim - x3, info.imag, info.mont);
      p[0] = add_lim_scalar<lazy>(u0, u1, lim);
      p[1] = sub_lim_scalar<lazy>(u0, u1, lim);
      p[2] = add_lim_scalar<lazy>(u2, u3, lim);
      p[3] = sub_lim_scalar<lazy>(u2, u3, lim);
    };
    if (blocks == 1) {
      bfly4(a, w1);
      return;
    }
    unsigned int w1_0 = w1;
    unsigned int w1_1 = mont_mul_scalar<lazy>(w1, info.step[1][0], info.mont);
    int u_end = blocks >> 1;
    for (int u = 0; u < u_end; ++u) {
      unsigned int* p = a + (u << 3);
      bfly4(p, w1_0);
      bfly4(p + 4, w1_1);
      if (u + 1 < u_end) {
        unsigned int st = info.step[2][std::countr_one((unsigned int)u)];
        w1_0 = mont_mul_scalar<lazy>(w1_0, st, info.mont);
        w1_1 = mont_mul_scalar<lazy>(w1_1, st, info.mont);
      }
    }
    return;
  }
  int q = FixedQ > 0 ? FixedQ : (1 << (h - 2));
  int blocks = FixedQ > 0 ? (n / (FixedQ << 2)) : (n >> h);
  for (int s = 0; s < blocks; ++s) {
    unsigned int w2 = mont_mul_scalar<lazy>(w1, w1, info.mont);
    unsigned int w3 = mont_mul_scalar<lazy>(w1, w2, info.mont);
    unsigned int* p0 = a + (FixedQ > 0 ? s * (FixedQ << 2) : (s << h));
    unsigned int* p1 = p0 + q;
    unsigned int* p2 = p1 + q;
    unsigned int* p3 = p2 + q;
    for (int i = 0; i < q; ++i) {
      unsigned int a0 = p0[i];
      unsigned int x1 = mont_mul_scalar<lazy>(p1[i], w1, info.mont);
      unsigned int x2 = mont_mul_scalar<lazy>(p2[i], w2, info.mont);
      unsigned int x3 = mont_mul_scalar<lazy>(p3[i], w3, info.mont);

      unsigned int u0 = add_lim_scalar<lazy>(a0, x2, lim);
      unsigned int u2 = sub_lim_scalar<lazy>(a0, x2, lim);
      unsigned int u1 = add_lim_scalar<lazy>(x1, x3, lim);
      unsigned int u3 =
          mont_mul_scalar<lazy>(x1 + lim - x3, info.imag, info.mont);

      p0[i] = add_lim_scalar<lazy>(u0, u1, lim);
      p1[i] = sub_lim_scalar<lazy>(u0, u1, lim);
      p2[i] = add_lim_scalar<lazy>(u2, u3, lim);
      p3[i] = sub_lim_scalar<lazy>(u2, u3, lim);
    }
    if (s + 1 < blocks) {
      w1 = mont_mul_scalar<lazy>(
          w1, info.step[1][std::countr_one((unsigned int)s)], info.mont);
    }
  }
}

template <int m, int FixedQ = 0>
inline void radix4_stage_scalar_inv(unsigned int* a, int n, int h,
                                    unsigned int iw1) {
  const auto& info = ntt_info<m>;
  constexpr bool lazy = NttInfo<m>::lazy;
  constexpr unsigned int lim = lazy ? NttInfo<m>::mod * 2U : NttInfo<m>::mod;
  if constexpr (FixedQ == 1) {
    int blocks = n >> 2;
    auto bfly4_inv = [&](unsigned int* p, unsigned int iw1_s) {
      unsigned int iw2_s = mont_mul_scalar<lazy>(iw1_s, iw1_s, info.mont);
      unsigned int iw3_s = mont_mul_scalar<lazy>(iw1_s, iw2_s, info.mont);
      unsigned int c0 = p[0];
      unsigned int c1 = p[1];
      unsigned int c2 = p[2];
      unsigned int c3 = p[3];
      unsigned int u0 = add_lim_scalar<lazy>(c0, c1, lim);
      unsigned int u1 = sub_lim_scalar<lazy>(c0, c1, lim);
      unsigned int u2 = add_lim_scalar<lazy>(c2, c3, lim);
      unsigned int u3 =
          mont_mul_scalar<lazy>(c2 + lim - c3, info.iimag, info.mont);
      p[0] = add_lim_scalar<lazy>(u0, u2, lim);
      p[2] = mont_mul_scalar<lazy>(u0 + lim - u2, iw2_s, info.mont);
      p[1] = mont_mul_scalar<lazy>(u1 + u3, iw1_s, info.mont);
      p[3] = mont_mul_scalar<lazy>(u1 + lim - u3, iw3_s, info.mont);
    };
    if (blocks == 1) {
      bfly4_inv(a, iw1);
      return;
    }
    unsigned int iw1_0 = iw1;
    unsigned int iw1_1 =
        mont_mul_scalar<lazy>(iw1, info.istep[1][0], info.mont);
    int u_end = blocks >> 1;
    for (int u = 0; u < u_end; ++u) {
      unsigned int* p = a + (u << 3);
      bfly4_inv(p, iw1_0);
      bfly4_inv(p + 4, iw1_1);
      if (u + 1 < u_end) {
        unsigned int st = info.istep[2][std::countr_one((unsigned int)u)];
        iw1_0 = mont_mul_scalar<lazy>(iw1_0, st, info.mont);
        iw1_1 = mont_mul_scalar<lazy>(iw1_1, st, info.mont);
      }
    }
    return;
  }
  int q = FixedQ > 0 ? FixedQ : (1 << (h - 2));
  int blocks = FixedQ > 0 ? (n / (FixedQ << 2)) : (n >> h);
  for (int s = 0; s < blocks; ++s) {
    unsigned int iw2 = mont_mul_scalar<lazy>(iw1, iw1, info.mont);
    unsigned int iw3 = mont_mul_scalar<lazy>(iw1, iw2, info.mont);
    unsigned int* p0 = a + (FixedQ > 0 ? s * (FixedQ << 2) : (s << h));
    unsigned int* p1 = p0 + q;
    unsigned int* p2 = p1 + q;
    unsigned int* p3 = p2 + q;
    for (int i = 0; i < q; ++i) {
      unsigned int c0 = p0[i];
      unsigned int c1 = p1[i];
      unsigned int c2 = p2[i];
      unsigned int c3 = p3[i];

      unsigned int u0 = add_lim_scalar<lazy>(c0, c1, lim);
      unsigned int u1 = sub_lim_scalar<lazy>(c0, c1, lim);
      unsigned int u2 = add_lim_scalar<lazy>(c2, c3, lim);
      unsigned int u3 =
          mont_mul_scalar<lazy>(c2 + lim - c3, info.iimag, info.mont);

      p0[i] = add_lim_scalar<lazy>(u0, u2, lim);
      p2[i] = mont_mul_scalar<lazy>(u0 + lim - u2, iw2, info.mont);
      p1[i] = mont_mul_scalar<lazy>(u1 + u3, iw1, info.mont);
      p3[i] = mont_mul_scalar<lazy>(u1 + lim - u3, iw3, info.mont);
    }
    if (s + 1 < blocks) {
      iw1 = mont_mul_scalar<lazy>(
          iw1, info.istep[1][std::countr_one((unsigned int)s)], info.mont);
    }
  }
}

template <int m, bool HalfZero = false> void butterfly(unsigned int* a, int n) {
  const auto& info = ntt_info<m>;
  constexpr unsigned int mod = NttInfo<m>::mod;
  constexpr bool lazy = NttInfo<m>::lazy;
  constexpr unsigned int lim = lazy ? mod * 2U : mod;
  int log_n = std::countr_zero((unsigned int)n);

  int h = log_n;
  if (h & 1) {
    int p = n >> 1;
    if constexpr (HalfZero) {
      std::memcpy(a + p, a, sizeof(unsigned int) * (unsigned int)p);
    } else {
      for (int i = 0; i < p; ++i) {
        unsigned int x = a[i];
        unsigned int y = a[i + p];
        a[i] = add_lim_scalar<lazy>(x, y, lim);
        a[i + p] = sub_lim_scalar<lazy>(x, y, lim);
      }
    }
    --h;
  } else if constexpr (HalfZero) {
    if (n >= 4) {
      int q = n >> 2;
      unsigned int* p0 = a;
      unsigned int* p1 = a + q;
      unsigned int* p2 = p1 + q;
      unsigned int* p3 = p2 + q;
      for (int i = 0; i < q; ++i) {
        unsigned int a0 = p0[i];
        unsigned int a1 = p1[i];
        unsigned int u3 = mont_mul_scalar<lazy>(a1, info.imag, info.mont);
        p0[i] = add_lim_scalar<lazy>(a0, a1, lim);
        p1[i] = sub_lim_scalar<lazy>(a0, a1, lim);
        p2[i] = add_lim_scalar<lazy>(a0, u3, lim);
        p3[i] = sub_lim_scalar<lazy>(a0, u3, lim);
      }
      h -= 2;
    }
  }

  if (h <= 12) {
    for (; h >= 6; h -= 2) radix4_stage_scalar_fwd<m>(a, n, h, info.mont.one());
    if (h >= 4) radix4_stage_scalar_fwd<m, 4>(a, n, 4, info.mont.one());
    if (h >= 2) radix4_stage_scalar_fwd<m, 1>(a, n, 2, info.mont.one());
    return;
  }

  for (; h > 12; h -= 2) radix4_stage_scalar_fwd<m>(a, n, h, info.mont.one());

  unsigned int w_init[6] = {info.mont.one(), info.mont.one(), info.mont.one(),
                            info.mont.one(), info.mont.one(), info.mont.one()};
  int s_end = n >> 12;
  for (int s = 0; s < s_end; ++s) {
    unsigned int* blk = a + (s << 12);
    radix4_stage_scalar_fwd<m>(blk, 1 << 12, 12, w_init[0]);
    radix4_stage_scalar_fwd<m>(blk, 1 << 12, 10, w_init[1]);
    radix4_stage_scalar_fwd<m>(blk, 1 << 12, 8, w_init[2]);
    radix4_stage_scalar_fwd<m>(blk, 1 << 12, 6, w_init[3]);
    radix4_stage_scalar_fwd<m, 4>(blk, 1 << 12, 4, w_init[4]);
    radix4_stage_scalar_fwd<m, 1>(blk, 1 << 12, 2, w_init[5]);
    if (s + 1 < s_end) {
      int k = std::countr_one((unsigned int)s);
      w_init[0] = mont_mul_scalar<lazy>(w_init[0], info.step[1][k], info.mont);
      w_init[1] = mont_mul_scalar<lazy>(w_init[1], info.step[3][k], info.mont);
      w_init[2] = mont_mul_scalar<lazy>(w_init[2], info.step[5][k], info.mont);
      w_init[3] = mont_mul_scalar<lazy>(w_init[3], info.step[7][k], info.mont);
      w_init[4] = mont_mul_scalar<lazy>(w_init[4], info.step[9][k], info.mont);
      w_init[5] = mont_mul_scalar<lazy>(w_init[5], info.step[11][k], info.mont);
    }
  }
}

template <int m> void butterfly_inv(unsigned int* a, int n) {
  const auto& info = ntt_info<m>;
  constexpr unsigned int mod = NttInfo<m>::mod;
  constexpr bool lazy = NttInfo<m>::lazy;
  constexpr unsigned int lim = lazy ? mod * 2U : mod;
  int log_n = std::countr_zero((unsigned int)n);

  int h = 2;
  if (log_n <= 12) {
    if (2 <= log_n) radix4_stage_scalar_inv<m, 1>(a, n, 2, info.mont.one());
    if (4 <= log_n) radix4_stage_scalar_inv<m, 4>(a, n, 4, info.mont.one());
    for (h = 6; h <= log_n; h += 2) {
      radix4_stage_scalar_inv<m>(a, n, h, info.mont.one());
    }
    if ((log_n & 1) != 0) {
      int p = n >> 1;
      for (int i = 0; i < p; ++i) {
        unsigned int x = a[i];
        unsigned int y = a[i + p];
        a[i] = add_lim_scalar<lazy>(x, y, lim);
        a[i + p] = sub_lim_scalar<lazy>(x, y, lim);
      }
    }
    return;
  }

  unsigned int iw_init[6] = {info.mont.one(), info.mont.one(), info.mont.one(),
                             info.mont.one(), info.mont.one(), info.mont.one()};
  int s_end = n >> 12;
  for (int s = 0; s < s_end; ++s) {
    unsigned int* blk = a + (s << 12);
    radix4_stage_scalar_inv<m, 1>(blk, 1 << 12, 2, iw_init[5]);
    radix4_stage_scalar_inv<m, 4>(blk, 1 << 12, 4, iw_init[4]);
    radix4_stage_scalar_inv<m>(blk, 1 << 12, 6, iw_init[3]);
    radix4_stage_scalar_inv<m>(blk, 1 << 12, 8, iw_init[2]);
    radix4_stage_scalar_inv<m>(blk, 1 << 12, 10, iw_init[1]);
    radix4_stage_scalar_inv<m>(blk, 1 << 12, 12, iw_init[0]);
    if (s + 1 < s_end) {
      int k = std::countr_one((unsigned int)s);
      iw_init[0] =
          mont_mul_scalar<lazy>(iw_init[0], info.istep[1][k], info.mont);
      iw_init[1] =
          mont_mul_scalar<lazy>(iw_init[1], info.istep[3][k], info.mont);
      iw_init[2] =
          mont_mul_scalar<lazy>(iw_init[2], info.istep[5][k], info.mont);
      iw_init[3] =
          mont_mul_scalar<lazy>(iw_init[3], info.istep[7][k], info.mont);
      iw_init[4] =
          mont_mul_scalar<lazy>(iw_init[4], info.istep[9][k], info.mont);
      iw_init[5] =
          mont_mul_scalar<lazy>(iw_init[5], info.istep[11][k], info.mont);
    }
  }

  for (h = 14; h <= log_n; h += 2) {
    radix4_stage_scalar_inv<m>(a, n, h, info.mont.one());
  }
  if (log_n & 1) {
    int p = n >> 1;
    for (int i = 0; i < p; ++i) {
      unsigned int x = a[i];
      unsigned int y = a[i + p];
      a[i] = add_lim_scalar<lazy>(x, y, lim);
      a[i + p] = sub_lim_scalar<lazy>(x, y, lim);
    }
  }
}

template <int m>
void pointwise_mul(unsigned int* a, const unsigned int* b, int n) {
  for (int i = 0; i < n; ++i) {
    a[i] = mont_mul_scalar<NttInfo<m>::lazy>(a[i], b[i], ntt_info<m>.mont);
  }
}

template <int m, typename OutPtr, typename Transform>
void scale_out_impl(const unsigned int* src, OutPtr dst, int n,
                    unsigned int factor, Transform&& transform) {
  for (int i = 0; i < n; ++i) {
    dst[i] = transform(ntt_info<m>.mont.mul(src[i], factor));
  }
}

#endif  // __AVX2__

}  // namespace internal_ntt

template <int m> void ntt_forward_prepared(unsigned int* dst, int n, int z) {
  int half = z >> 1;
  if (z >= 4 && n <= half) {
    if (n < half) {
      std::memset(dst + n, 0, sizeof(unsigned int) * (unsigned int)(half - n));
    }
    internal_ntt::butterfly<m, true>(dst, z);
  } else {
    if (n < z) {
      std::memset(dst + n, 0, sizeof(unsigned int) * (unsigned int)(z - n));
    }
    internal_ntt::butterfly<m, false>(dst, z);
  }
}

template <int m>
unsigned int finish_ntt_convolution(unsigned int* a, unsigned int* b, int z,
                                    bool same_input) {
  if (same_input) {
    internal_ntt::pointwise_mul<m>(a, a, z);
  } else {
    internal_ntt::pointwise_mul<m>(a, b, z);
  }
  internal_ntt::butterfly_inv<m>(a, z);

  const auto& info = ntt_info<m>;
  unsigned int iz_mont =
      info.mont.pow(info.mont.to_mont((unsigned int)z), info.mod - 2);
  return info.mont.mul(iz_mont, info.mont.r2());
}

template <int m, typename OutPtr, typename Transform>
void ntt_scale_out(const unsigned int* src, OutPtr dst, int n,
                   unsigned int factor, Transform&& transform) {
  internal_ntt::scale_out_impl<m>(src, dst, n, factor,
                                  std::forward<Transform>(transform));
}

template <int m>
void ntt_scale_out(const unsigned int* src, unsigned int* dst, int n,
                   unsigned int factor) {
  internal_ntt::scale_out_impl<m>(
      src, dst, n, factor, [](unsigned int v) -> unsigned int { return v; });
}

}  // namespace library3372::internal

#endif  // LIBRARY3372_LIBRARY3372_INTERNAL_MATH_NTT_HPP_
