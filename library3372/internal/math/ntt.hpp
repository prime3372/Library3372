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

namespace library3372::internal {

inline constexpr int ntt_naive_threshold = 60;

namespace internal_ntt {

using u32 = unsigned int;

constexpr bool is_prime32(int m) {
  if (m < 2) return false;
  if (m == 2) return true;
  if ((m & 1) == 0) return false;
  for (u32 d = 3; d * d <= (u32)m; d += 2) {
    if ((u32)m % d == 0) return false;
  }
  return true;
}

constexpr int ntt_primitive_root(int m) {
  if (m == 2) return 1;
  u32 divs[16] = {};
  int cnt = 0;
  u32 x = (u32)m - 1U;
  for (u32 i = 2; i * i <= x; ++i) {
    if (x % i != 0) continue;
    divs[cnt++] = i;
    while (x % i == 0) x /= i;
  }
  if (x > 1) divs[cnt++] = x;
  Montgomery32 mont((u32)m);
  for (u32 g = 2;; ++g) {
    u32 gm = mont.to_mont(g);
    bool ok = true;
    for (int i = 0; i < cnt && ok; ++i) {
      ok = mont.pow(gm, ((u32)m - 1U) / divs[i]) != mont.one();
    }
    if (ok) return (int)g;
  }
}

}  // namespace internal_ntt

template <int m> struct NttInfo {
  using u32 = internal_ntt::u32;
  static constexpr bool valid = (m >= 2 && internal_ntt::is_prime32(m));
  static constexpr u32 mod = (u32)(valid ? m : 3);
  static constexpr bool lazy = (mod < (1U << 30));
  static constexpr int rank2 = std::countr_zero(mod - 1);
  static constexpr Montgomery32 mont{mod == 2 ? 3U : mod};

  u32 imag = 0;
  u32 iimag = 0;
  u32 step[13][32] = {};
  u32 istep[13][32] = {};
  alignas(32) u32 w4_init[8] = {};
  alignas(32) u32 w2_init[8] = {};
  alignas(32) u32 w1_init[8] = {};
  alignas(32) u32 iw4_init[8] = {};
  alignas(32) u32 iw2_init[8] = {};
  alignas(32) u32 iw1_init[8] = {};

  constexpr NttInfo() {
    if constexpr (!valid || mod == 2) return;
    int g = internal_ntt::ntt_primitive_root((int)mod);
    u32 g_mont = mont.to_mont((u32)g);

    u32 root[32] = {};
    u32 iroot[32] = {};
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
      u32 prod = mont.one();
      u32 iprod = mont.one();
      for (int k = 0; k + d <= rank2 - 2; ++k) {
        step[d][k] = mont.mul(root[k + d + 2], prod);
        istep[d][k] = mont.mul(iroot[k + d + 2], iprod);
        prod = mont.mul(prod, iroot[k + d + 2]);
        iprod = mont.mul(iprod, root[k + d + 2]);
      }
    }

    if (rank2 >= 4) {
      auto fill_init = [&](const u32* r, u32* w4, u32* w2, u32* w1) {
        u32 e0 = r[2], e1 = r[3], e2 = r[4];
        u32 e01 = mont.mul(e0, e1), e02 = mont.mul(e0, e2);
        u32 e12 = mont.mul(e1, e2), e012 = mont.mul(e01, e2);
        u32 t4[8] = {mont.one(), mont.one(), mont.one(), mont.one(),
                     e0,         e0,         e0,         e0};
        u32 t2[8] = {mont.one(), mont.one(), e0, e0, e1, e1, e01, e01};
        u32 t1[8] = {mont.one(), e1, e0, e01, e2, e12, e02, e012};
        for (int i = 0; i < 8; ++i) {
          w4[i] = t4[i];
          w2[i] = t2[i];
          w1[i] = t1[i];
        }
      };
      fill_init(root, w4_init, w2_init, w1_init);
      fill_init(iroot, iw4_init, iw2_init, iw1_init);
    }
  }
};

template <int m> inline constexpr NttInfo<m> ntt_info{};

template <typename T> struct NttAlignedBufferBase {
  T* data = nullptr;

  NttAlignedBufferBase() = default;
  explicit NttAlignedBufferBase(int n)
      : data(n > 0 ? new (std::align_val_t(32)) T[n] : nullptr) {}

  ~NttAlignedBufferBase() {
    if (data != nullptr) ::operator delete[](data, std::align_val_t(32));
  }

  NttAlignedBufferBase(const NttAlignedBufferBase&) = delete;
  NttAlignedBufferBase& operator=(const NttAlignedBufferBase&) = delete;

  NttAlignedBufferBase(NttAlignedBufferBase&& other) noexcept
      : data(other.data) {
    other.data = nullptr;
  }

  NttAlignedBufferBase& operator=(NttAlignedBufferBase&& other) noexcept {
    if (this != &other) {
      if (data != nullptr) ::operator delete[](data, std::align_val_t(32));
      data = other.data;
      other.data = nullptr;
    }
    return *this;
  }
};

using NttAlignedBuffer = NttAlignedBufferBase<unsigned int>;
using NttAlignedBuffer64 = NttAlignedBufferBase<unsigned long long>;

namespace internal_ntt {

inline u32 add_lim(u32 a, u32 b, u32 lim) {
  u32 s = a + b;
  return s >= lim ? s - lim : s;
}

inline u32 sub_lim(u32 a, u32 b, u32 lim) {
  return a >= b ? a - b : a + lim - b;
}

template <bool Lazy> inline u32 mmul(u32 a, u32 b, const Montgomery32& mont) {
  if constexpr (Lazy) return mont.mul_2m(a, b);
  return mont.mul(a, b);
}

#ifdef __AVX2__

using Vec = __m256i;

inline Vec vload(const u32* p) { return _mm256_load_si256((const Vec*)p); }
inline void vstore(u32* p, Vec v) { _mm256_store_si256((Vec*)p, v); }
inline Vec vbcast(u32 x) { return _mm256_set1_epi32((int)x); }
inline Vec vadd(Vec a, Vec b) { return _mm256_add_epi32(a, b); }
inline Vec vsub(Vec a, Vec b) { return _mm256_sub_epi32(a, b); }

struct MontConsts {
  Vec mod, lim, inv;

  static MontConsts create(u32 m, u32 limit) {
    return {vbcast(m), vbcast(limit), vbcast(Montgomery32(m).neg_inv())};
  }
};

inline Vec add_lim(Vec a, Vec b, Vec lim) {
  Vec s = vadd(a, b);
  return _mm256_min_epu32(s, vsub(s, lim));
}

inline Vec sub_lim(Vec a, Vec b, Vec lim) {
  Vec d = vsub(a, b);
  return _mm256_min_epu32(d, vadd(d, lim));
}

template <bool Lazy> inline Vec mmul(Vec a, Vec b, const MontConsts& c) {
  Vec a_odd = _mm256_srli_epi64(a, 32);
  Vec b_odd = _mm256_srli_epi64(b, 32);
  Vec t_e = _mm256_mul_epu32(a, b);
  Vec t_o = _mm256_mul_epu32(a_odd, b_odd);
  Vec q_e = _mm256_mul_epu32(t_e, c.inv);
  Vec q_o = _mm256_mul_epu32(t_o, c.inv);
  Vec qm_e = _mm256_mul_epu32(q_e, c.mod);
  Vec qm_o = _mm256_mul_epu32(q_o, c.mod);
  Vec s_e = _mm256_add_epi64(t_e, qm_e);
  Vec s_o = _mm256_add_epi64(t_o, qm_o);
  Vec u = _mm256_blend_epi32(_mm256_srli_epi64(s_e, 32), s_o, 0xAA);
  if constexpr (!Lazy) u = _mm256_min_epu32(u, vsub(u, c.mod));
  return u;
}

template <bool Lazy>
inline Vec mmul_b(Vec a, Vec b_bcast, const MontConsts& c) {
  Vec a_odd = _mm256_srli_epi64(a, 32);
  Vec t_e = _mm256_mul_epu32(a, b_bcast);
  Vec t_o = _mm256_mul_epu32(a_odd, b_bcast);
  Vec q_e = _mm256_mul_epu32(t_e, c.inv);
  Vec q_o = _mm256_mul_epu32(t_o, c.inv);
  Vec qm_e = _mm256_mul_epu32(q_e, c.mod);
  Vec qm_o = _mm256_mul_epu32(q_o, c.mod);
  Vec s_e = _mm256_add_epi64(t_e, qm_e);
  Vec s_o = _mm256_add_epi64(t_o, qm_o);
  Vec u = _mm256_blend_epi32(_mm256_srli_epi64(s_e, 32), s_o, 0xAA);
  if constexpr (!Lazy) u = _mm256_min_epu32(u, vsub(u, c.mod));
  return u;
}

template <bool Lazy> inline Vec mmul_b(Vec a, u32 b, const MontConsts& c) {
  return mmul_b<Lazy>(a, vbcast(b), c);
}

template <int m, int FixedQ = 0>
inline void r4_fwd(u32* a, int n, int h, u32 w1, const MontConsts& c,
                   Vec vimag) {
  const auto& info = ntt_info<m>;
  constexpr bool lazy = NttInfo<m>::lazy;
  int q = FixedQ > 0 ? FixedQ : (1 << (h - 2));
  int blocks = FixedQ > 0 ? (n / (FixedQ << 2)) : (n >> h);
  for (int s = 0; s < blocks; ++s) {
    u32 w2 = mmul<lazy>(w1, w1, info.mont);
    u32 w3 = mmul<lazy>(w1, w2, info.mont);
    Vec vw1 = vbcast(w1), vw2 = vbcast(w2), vw3 = vbcast(w3);
    u32* p0 = a + (FixedQ > 0 ? s * (FixedQ << 2) : (s << h));
    u32 *p1 = p0 + q, *p2 = p1 + q, *p3 = p2 + q;
    for (int i = 0; i < q; i += 8) {
      Vec a0 = vload(p0 + i);
      Vec x1 = mmul_b<lazy>(vload(p1 + i), vw1, c);
      Vec x2 = mmul_b<lazy>(vload(p2 + i), vw2, c);
      Vec x3 = mmul_b<lazy>(vload(p3 + i), vw3, c);

      Vec u0 = add_lim(a0, x2, c.lim);
      Vec u2 = sub_lim(a0, x2, c.lim);
      Vec u1 = add_lim(x1, x3, c.lim);
      Vec u3 = mmul_b<lazy>(vsub(vadd(x1, c.lim), x3), vimag, c);

      vstore(p0 + i, add_lim(u0, u1, c.lim));
      vstore(p1 + i, sub_lim(u0, u1, c.lim));
      vstore(p2 + i, add_lim(u2, u3, c.lim));
      vstore(p3 + i, sub_lim(u2, u3, c.lim));
    }
    if (s + 1 < blocks) {
      w1 = mmul<lazy>(w1, info.step[1][std::countr_one((u32)s)], info.mont);
    }
  }
}

template <int m>
inline void last3_fwd_pair(u32* ptr, Vec w4, Vec w2, Vec w1,
                           const MontConsts& c) {
  constexpr bool lazy = NttInfo<m>::lazy;
  Vec v0 = vload(ptr), v1 = vload(ptr + 8);

  Vec lo = _mm256_permute2x128_si256(v0, v1, 0x20);
  Vec hi = _mm256_permute2x128_si256(v0, v1, 0x31);

  Vec r4 = mmul<lazy>(hi, w4, c);
  Vec a4 = add_lim(lo, r4, c.lim);
  Vec b4 = sub_lim(lo, r4, c.lim);

  Vec x0 = _mm256_unpacklo_epi64(a4, b4);
  Vec x1 = _mm256_unpackhi_epi64(a4, b4);

  Vec r2 = mmul<lazy>(x1, w2, c);
  Vec a2 = add_lim(x0, r2, c.lim);
  Vec b2 = sub_lim(x0, r2, c.lim);

  Vec y0 = _mm256_castps_si256(_mm256_shuffle_ps(
      _mm256_castsi256_ps(a2), _mm256_castsi256_ps(b2), 0x88));
  Vec y1 = _mm256_castps_si256(_mm256_shuffle_ps(
      _mm256_castsi256_ps(a2), _mm256_castsi256_ps(b2), 0xDD));

  Vec r1 = mmul<lazy>(y1, w1, c);
  Vec a1 = add_lim(y0, r1, c.lim);
  Vec b1 = sub_lim(y0, r1, c.lim);

  // Skip bit-reversal unshuffling here: pointwise multiplication commutes with
  // permutation, and last3_inv_pair consumes this [a1, b1] layout directly.
  vstore(ptr, a1);
  vstore(ptr + 8, b1);
}

template <int m>
inline void last3_fwd(u32* a, int t_end, Vec w4_0, Vec w2_0, Vec w1_0,
                      const MontConsts& c) {
  if (t_end == 1) {
    last3_fwd_pair<m>(a, w4_0, w2_0, w1_0, c);
    return;
  }
  const auto& info = ntt_info<m>;
  constexpr bool lazy = NttInfo<m>::lazy;
  Vec w4_1 = mmul_b<lazy>(w4_0, info.step[1][0], c);
  Vec w2_1 = mmul_b<lazy>(w2_0, info.step[2][0], c);
  Vec w1_1 = mmul_b<lazy>(w1_0, info.step[3][0], c);
  int u_end = t_end >> 1;
  for (int u = 0; u < u_end; ++u) {
    u32* ptr = a + (u << 5);
    last3_fwd_pair<m>(ptr, w4_0, w2_0, w1_0, c);
    last3_fwd_pair<m>(ptr + 16, w4_1, w2_1, w1_1, c);
    if (u + 1 < u_end) {
      int k = std::countr_one((u32)u);
      Vec s4 = vbcast(info.step[2][k]);
      Vec s2 = vbcast(info.step[3][k]);
      Vec s1 = vbcast(info.step[4][k]);
      w4_0 = mmul_b<lazy>(w4_0, s4, c);
      w4_1 = mmul_b<lazy>(w4_1, s4, c);
      w2_0 = mmul_b<lazy>(w2_0, s2, c);
      w2_1 = mmul_b<lazy>(w2_1, s2, c);
      w1_0 = mmul_b<lazy>(w1_0, s1, c);
      w1_1 = mmul_b<lazy>(w1_1, s1, c);
    }
  }
}

template <int m, int FixedQ = 0>
inline void r4_inv(u32* a, int n, int h, u32 iw1_s, const MontConsts& c,
                   Vec viimag) {
  const auto& info = ntt_info<m>;
  constexpr bool lazy = NttInfo<m>::lazy;
  int q = FixedQ > 0 ? FixedQ : (1 << (h - 2));
  int blocks = FixedQ > 0 ? (n / (FixedQ << 2)) : (n >> h);
  for (int s = 0; s < blocks; ++s) {
    u32 iw2_s = mmul<lazy>(iw1_s, iw1_s, info.mont);
    u32 iw3_s = mmul<lazy>(iw1_s, iw2_s, info.mont);
    Vec vw1 = vbcast(iw1_s), vw2 = vbcast(iw2_s), vw3 = vbcast(iw3_s);
    u32* p0 = a + (FixedQ > 0 ? s * (FixedQ << 2) : (s << h));
    u32 *p1 = p0 + q, *p2 = p1 + q, *p3 = p2 + q;
    for (int i = 0; i < q; i += 8) {
      Vec c0 = vload(p0 + i), c1 = vload(p1 + i);
      Vec c2 = vload(p2 + i), c3 = vload(p3 + i);

      Vec u0 = add_lim(c0, c1, c.lim);
      Vec u1 = sub_lim(c0, c1, c.lim);
      Vec u2 = add_lim(c2, c3, c.lim);
      Vec u3 = mmul_b<lazy>(vsub(vadd(c2, c.lim), c3), viimag, c);

      vstore(p0 + i, add_lim(u0, u2, c.lim));
      vstore(p2 + i, mmul_b<lazy>(vsub(vadd(u0, c.lim), u2), vw2, c));
      vstore(p1 + i, mmul_b<lazy>(vadd(u1, u3), vw1, c));
      vstore(p3 + i, mmul_b<lazy>(vsub(vadd(u1, c.lim), u3), vw3, c));
    }
    if (s + 1 < blocks) {
      iw1_s =
          mmul<lazy>(iw1_s, info.istep[1][std::countr_one((u32)s)], info.mont);
    }
  }
}

template <int m, bool SameInput>
inline void last3_inv_pair(u32* ptr, const u32* b_ptr, Vec iw4, Vec iw2,
                           Vec iw1, const MontConsts& c) {
  constexpr bool lazy = NttInfo<m>::lazy;
  Vec y0 = vload(ptr), y1 = vload(ptr + 8);
  if constexpr (SameInput) {
    y0 = mmul<lazy>(y0, y0, c);
    y1 = mmul<lazy>(y1, y1, c);
  } else {
    y0 = mmul<lazy>(y0, vload(b_ptr), c);
    y1 = mmul<lazy>(y1, vload(b_ptr + 8), c);
  }

  Vec a1 = add_lim(y0, y1, c.lim);
  Vec b1 = mmul<lazy>(vsub(vadd(y0, c.lim), y1), iw1, c);

  Vec z0 = _mm256_unpacklo_epi32(a1, b1);
  Vec z1 = _mm256_unpackhi_epi32(a1, b1);

  Vec a2 = add_lim(z0, z1, c.lim);
  Vec b2 = mmul<lazy>(vsub(vadd(z0, c.lim), z1), iw2, c);

  Vec lo4 = _mm256_unpacklo_epi64(a2, b2);
  Vec hi4 = _mm256_unpackhi_epi64(a2, b2);

  Vec a4 = add_lim(lo4, hi4, c.lim);
  Vec b4 = mmul<lazy>(vsub(vadd(lo4, c.lim), hi4), iw4, c);

  vstore(ptr, _mm256_permute2x128_si256(a4, b4, 0x20));
  vstore(ptr + 8, _mm256_permute2x128_si256(a4, b4, 0x31));
}

template <int m, bool SameInput>
inline void last3_inv(u32* a, const u32* b, int t_end, Vec iw4_0, Vec iw2_0,
                      Vec iw1_0, const MontConsts& c) {
  if (t_end == 1) {
    last3_inv_pair<m, SameInput>(a, b, iw4_0, iw2_0, iw1_0, c);
    return;
  }
  const auto& info = ntt_info<m>;
  constexpr bool lazy = NttInfo<m>::lazy;
  Vec iw4_1 = mmul_b<lazy>(iw4_0, info.istep[1][0], c);
  Vec iw2_1 = mmul_b<lazy>(iw2_0, info.istep[2][0], c);
  Vec iw1_1 = mmul_b<lazy>(iw1_0, info.istep[3][0], c);
  int u_end = t_end >> 1;
  for (int u = 0; u < u_end; ++u) {
    int off = u << 5;
    u32* ptr = a + off;
    const u32* b_ptr = SameInput ? nullptr : b + off;
    last3_inv_pair<m, SameInput>(ptr, b_ptr, iw4_0, iw2_0, iw1_0, c);
    last3_inv_pair<m, SameInput>(ptr + 16, SameInput ? nullptr : b_ptr + 16,
                                 iw4_1, iw2_1, iw1_1, c);
    if (u + 1 < u_end) {
      int k = std::countr_one((u32)u);
      Vec s4 = vbcast(info.istep[2][k]);
      Vec s2 = vbcast(info.istep[3][k]);
      Vec s1 = vbcast(info.istep[4][k]);
      iw4_0 = mmul_b<lazy>(iw4_0, s4, c);
      iw4_1 = mmul_b<lazy>(iw4_1, s4, c);
      iw2_0 = mmul_b<lazy>(iw2_0, s2, c);
      iw2_1 = mmul_b<lazy>(iw2_1, s2, c);
      iw1_0 = mmul_b<lazy>(iw1_0, s1, c);
      iw1_1 = mmul_b<lazy>(iw1_1, s1, c);
    }
  }
}

template <int m, bool HalfZero = false> void butterfly(u32* a, int n) {
  const auto& info = ntt_info<m>;
  constexpr u32 mod = NttInfo<m>::mod;
  constexpr bool lazy = NttInfo<m>::lazy;
  constexpr u32 lim = lazy ? mod * 2U : mod;
  const MontConsts c = MontConsts::create(mod, lim);
  int log_n = std::countr_zero((u32)n);

  if (n < 16) {
    int start_len = n >> 1;
    if constexpr (HalfZero) {
      std::memcpy(a + start_len, a, sizeof(u32) * (u32)start_len);
      start_len >>= 1;
    }
    for (int len = start_len; len >= 1; len >>= 1) {
      int blocks = n / (len << 1);
      u32 rot = info.mont.one();
      for (int s = 0; s < blocks; ++s) {
        int off = s * (len << 1);
        for (int i = 0; i < len; ++i) {
          u32 l = a[off + i], r = info.mont.mul(a[off + i + len], rot);
          a[off + i] = add_lim(l, r, mod);
          a[off + i + len] = sub_lim(l, r, mod);
        }
        if (s + 1 < blocks) {
          rot = info.mont.mul(rot, info.step[0][std::countr_one((u32)s)]);
        }
      }
    }
    return;
  }

  const Vec vimag = vbcast(info.imag);
  int h = log_n;
  if ((h - 3) & 1) {
    int p = n >> 1;
    if constexpr (HalfZero) {
      std::memcpy(a + p, a, sizeof(u32) * (u32)p);
    } else {
      for (int i = 0; i < p; i += 8) {
        Vec x = vload(a + i), y = vload(a + i + p);
        vstore(a + i, add_lim(x, y, c.lim));
        vstore(a + i + p, sub_lim(x, y, c.lim));
      }
    }
    --h;
  } else if constexpr (HalfZero) {
    int q = n >> 2;
    u32 *p0 = a, *p1 = a + q, *p2 = p1 + q, *p3 = p2 + q;
    for (int i = 0; i < q; i += 8) {
      Vec a0 = vload(p0 + i), a1 = vload(p1 + i);
      Vec u3 = mmul_b<lazy>(a1, vimag, c);
      vstore(p0 + i, add_lim(a0, a1, c.lim));
      vstore(p1 + i, sub_lim(a0, a1, c.lim));
      vstore(p2 + i, add_lim(a0, u3, c.lim));
      vstore(p3 + i, sub_lim(a0, u3, c.lim));
    }
    h -= 2;
  }

  Vec w4 = vload(info.w4_init), w2 = vload(info.w2_init),
      w1 = vload(info.w1_init);

  if (h <= 13) {
    for (; h > 5; h -= 2) r4_fwd<m>(a, n, h, info.mont.one(), c, vimag);
    if (h == 5) r4_fwd<m, 8>(a, n, 5, info.mont.one(), c, vimag);
    last3_fwd<m>(a, n >> 4, w4, w2, w1, c);
    return;
  }

  for (; h > 13; h -= 2) r4_fwd<m>(a, n, h, info.mont.one(), c, vimag);

  u32 one = info.mont.one();
  u32 w_init[5] = {one, one, one, one, one};
  int s_end = n >> 13;
  for (int s = 0; s < s_end; ++s) {
    u32* blk = a + (s << 13);
    r4_fwd<m>(blk, 1 << 13, 13, w_init[0], c, vimag);
    r4_fwd<m>(blk, 1 << 13, 11, w_init[1], c, vimag);
    r4_fwd<m>(blk, 1 << 13, 9, w_init[2], c, vimag);
    r4_fwd<m>(blk, 1 << 13, 7, w_init[3], c, vimag);
    r4_fwd<m, 8>(blk, 1 << 13, 5, w_init[4], c, vimag);
    last3_fwd<m>(blk, 1 << 9, w4, w2, w1, c);
    if (s + 1 < s_end) {
      int k = std::countr_one((u32)s);
      for (int j = 0; j < 5; ++j) {
        w_init[j] = mmul<lazy>(w_init[j], info.step[2 * j + 1][k], info.mont);
      }
      w4 = mmul_b<lazy>(w4, info.step[10][k], c);
      w2 = mmul_b<lazy>(w2, info.step[11][k], c);
      w1 = mmul_b<lazy>(w1, info.step[12][k], c);
    }
  }
}

template <int m, bool SameInput>
void butterfly_inv(u32* a, const u32* b, int n) {
  const auto& info = ntt_info<m>;
  constexpr u32 mod = NttInfo<m>::mod;
  constexpr bool lazy = NttInfo<m>::lazy;
  constexpr u32 lim = lazy ? mod * 2U : mod;
  const MontConsts c = MontConsts::create(mod, lim);
  int log_n = std::countr_zero((u32)n);

  if (n < 16) {
    for (int i = 0; i < n; ++i)
      a[i] = info.mont.mul(a[i], SameInput ? a[i] : b[i]);
    for (int len = 1; len < n; len <<= 1) {
      int blocks = n / (len << 1);
      u32 irot = info.mont.one();
      for (int s = 0; s < blocks; ++s) {
        int off = s * (len << 1);
        for (int i = 0; i < len; ++i) {
          u32 l = a[off + i], r = a[off + i + len];
          a[off + i] = add_lim(l, r, mod);
          a[off + i + len] = info.mont.mul(sub_lim(l, r, mod), irot);
        }
        if (s + 1 < blocks) {
          irot = info.mont.mul(irot, info.istep[0][std::countr_one((u32)s)]);
        }
      }
    }
    return;
  }

  const Vec viimag = vbcast(info.iimag);
  Vec iw4 = vload(info.iw4_init), iw2 = vload(info.iw2_init),
      iw1 = vload(info.iw1_init);

  if (log_n <= 13) {
    last3_inv<m, SameInput>(a, b, n >> 4, iw4, iw2, iw1, c);
    int h = 5;
    if (h <= log_n) {
      r4_inv<m, 8>(a, n, 5, info.mont.one(), c, viimag);
      h = 7;
    }
    for (; h <= log_n; h += 2) r4_inv<m>(a, n, h, info.mont.one(), c, viimag);
    if (h - 1 == log_n) {
      int p = n >> 1;
      for (int i = 0; i < p; i += 8) {
        Vec x = vload(a + i), y = vload(a + i + p);
        vstore(a + i, add_lim(x, y, c.lim));
        vstore(a + i + p, sub_lim(x, y, c.lim));
      }
    }
    return;
  }

  u32 one = info.mont.one();
  u32 iw_init[5] = {one, one, one, one, one};
  int s_end = n >> 13;
  for (int s = 0; s < s_end; ++s) {
    int blk_off = s << 13;
    u32* blk = a + blk_off;
    last3_inv<m, SameInput>(blk, SameInput ? nullptr : b + blk_off, 1 << 9, iw4,
                            iw2, iw1, c);
    r4_inv<m, 8>(blk, 1 << 13, 5, iw_init[4], c, viimag);
    r4_inv<m>(blk, 1 << 13, 7, iw_init[3], c, viimag);
    r4_inv<m>(blk, 1 << 13, 9, iw_init[2], c, viimag);
    r4_inv<m>(blk, 1 << 13, 11, iw_init[1], c, viimag);
    r4_inv<m>(blk, 1 << 13, 13, iw_init[0], c, viimag);
    if (s + 1 < s_end) {
      int k = std::countr_one((u32)s);
      for (int j = 0; j < 5; ++j) {
        iw_init[j] =
            mmul<lazy>(iw_init[j], info.istep[2 * j + 1][k], info.mont);
      }
      iw4 = mmul_b<lazy>(iw4, info.istep[10][k], c);
      iw2 = mmul_b<lazy>(iw2, info.istep[11][k], c);
      iw1 = mmul_b<lazy>(iw1, info.istep[12][k], c);
    }
  }

  int h = 15;
  for (; h <= log_n; h += 2) r4_inv<m>(a, n, h, info.mont.one(), c, viimag);
  if (h - 1 == log_n) {
    int p = n >> 1;
    for (int i = 0; i < p; i += 8) {
      Vec x = vload(a + i), y = vload(a + i + p);
      vstore(a + i, add_lim(x, y, c.lim));
      vstore(a + i + p, sub_lim(x, y, c.lim));
    }
  }
}

template <int m, typename OutPtr, typename Transform>
void scale_out_impl(const u32* src, OutPtr dst, int n, u32 factor,
                    Transform&& transform) {
  constexpr u32 mod = NttInfo<m>::mod;
  const MontConsts c = MontConsts::create(mod, mod);
  Vec vf = vbcast(factor);
  int i = 0;
  for (; i + 8 <= n; i += 8) {
    Vec vr = mmul_b<false>(vload(src + i), vf, c);
    if constexpr (std::is_same_v<OutPtr, u32*>) {
      _mm256_storeu_si256((Vec*)(dst + i), vr);
    } else {
      alignas(32) u32 tmp[8];
      vstore(tmp, vr);
      for (int j = 0; j < 8; ++j) dst[i + j] = transform(tmp[j]);
    }
  }
  for (; i < n; ++i) dst[i] = transform(ntt_info<m>.mont.mul(src[i], factor));
}

#else

template <int m, int FixedQ = 0>
inline void r4_fwd(u32* a, int n, int h, u32 w1) {
  const auto& info = ntt_info<m>;
  constexpr bool lazy = NttInfo<m>::lazy;
  constexpr u32 lim = lazy ? NttInfo<m>::mod * 2U : NttInfo<m>::mod;
  if constexpr (FixedQ == 1) {
    int blocks = n >> 2;
    auto bfly4 = [&](u32* p, u32 w1_s) {
      u32 w2_s = mmul<lazy>(w1_s, w1_s, info.mont);
      u32 w3_s = mmul<lazy>(w1_s, w2_s, info.mont);
      u32 a0 = p[0];
      u32 x1 = mmul<lazy>(p[1], w1_s, info.mont);
      u32 x2 = mmul<lazy>(p[2], w2_s, info.mont);
      u32 x3 = mmul<lazy>(p[3], w3_s, info.mont);
      u32 u0 = add_lim(a0, x2, lim), u2 = sub_lim(a0, x2, lim);
      u32 u1 = add_lim(x1, x3, lim);
      u32 u3 = mmul<lazy>(x1 + lim - x3, info.imag, info.mont);
      p[0] = add_lim(u0, u1, lim);
      p[1] = sub_lim(u0, u1, lim);
      p[2] = add_lim(u2, u3, lim);
      p[3] = sub_lim(u2, u3, lim);
    };
    if (blocks == 1) {
      bfly4(a, w1);
      return;
    }
    u32 w1_0 = w1, w1_1 = mmul<lazy>(w1, info.step[1][0], info.mont);
    int u_end = blocks >> 1;
    for (int u = 0; u < u_end; ++u) {
      u32* p = a + (u << 3);
      bfly4(p, w1_0);
      bfly4(p + 4, w1_1);
      if (u + 1 < u_end) {
        u32 st = info.step[2][std::countr_one((u32)u)];
        w1_0 = mmul<lazy>(w1_0, st, info.mont);
        w1_1 = mmul<lazy>(w1_1, st, info.mont);
      }
    }
    return;
  }
  int q = FixedQ > 0 ? FixedQ : (1 << (h - 2));
  int blocks = FixedQ > 0 ? (n / (FixedQ << 2)) : (n >> h);
  for (int s = 0; s < blocks; ++s) {
    u32 w2 = mmul<lazy>(w1, w1, info.mont);
    u32 w3 = mmul<lazy>(w1, w2, info.mont);
    u32* p0 = a + (FixedQ > 0 ? s * (FixedQ << 2) : (s << h));
    u32 *p1 = p0 + q, *p2 = p1 + q, *p3 = p2 + q;
    for (int i = 0; i < q; ++i) {
      u32 a0 = p0[i];
      u32 x1 = mmul<lazy>(p1[i], w1, info.mont);
      u32 x2 = mmul<lazy>(p2[i], w2, info.mont);
      u32 x3 = mmul<lazy>(p3[i], w3, info.mont);
      u32 u0 = add_lim(a0, x2, lim), u2 = sub_lim(a0, x2, lim);
      u32 u1 = add_lim(x1, x3, lim);
      u32 u3 = mmul<lazy>(x1 + lim - x3, info.imag, info.mont);
      p0[i] = add_lim(u0, u1, lim);
      p1[i] = sub_lim(u0, u1, lim);
      p2[i] = add_lim(u2, u3, lim);
      p3[i] = sub_lim(u2, u3, lim);
    }
    if (s + 1 < blocks) {
      w1 = mmul<lazy>(w1, info.step[1][std::countr_one((u32)s)], info.mont);
    }
  }
}

template <int m, int FixedQ = 0>
inline void r4_inv(u32* a, int n, int h, u32 iw1) {
  const auto& info = ntt_info<m>;
  constexpr bool lazy = NttInfo<m>::lazy;
  constexpr u32 lim = lazy ? NttInfo<m>::mod * 2U : NttInfo<m>::mod;
  if constexpr (FixedQ == 1) {
    int blocks = n >> 2;
    auto bfly4_inv = [&](u32* p, u32 iw1_s) {
      u32 iw2_s = mmul<lazy>(iw1_s, iw1_s, info.mont);
      u32 iw3_s = mmul<lazy>(iw1_s, iw2_s, info.mont);
      u32 c0 = p[0], c1 = p[1], c2 = p[2], c3 = p[3];
      u32 u0 = add_lim(c0, c1, lim), u1 = sub_lim(c0, c1, lim);
      u32 u2 = add_lim(c2, c3, lim);
      u32 u3 = mmul<lazy>(c2 + lim - c3, info.iimag, info.mont);
      p[0] = add_lim(u0, u2, lim);
      p[2] = mmul<lazy>(u0 + lim - u2, iw2_s, info.mont);
      p[1] = mmul<lazy>(u1 + u3, iw1_s, info.mont);
      p[3] = mmul<lazy>(u1 + lim - u3, iw3_s, info.mont);
    };
    if (blocks == 1) {
      bfly4_inv(a, iw1);
      return;
    }
    u32 iw1_0 = iw1, iw1_1 = mmul<lazy>(iw1, info.istep[1][0], info.mont);
    int u_end = blocks >> 1;
    for (int u = 0; u < u_end; ++u) {
      u32* p = a + (u << 3);
      bfly4_inv(p, iw1_0);
      bfly4_inv(p + 4, iw1_1);
      if (u + 1 < u_end) {
        u32 st = info.istep[2][std::countr_one((u32)u)];
        iw1_0 = mmul<lazy>(iw1_0, st, info.mont);
        iw1_1 = mmul<lazy>(iw1_1, st, info.mont);
      }
    }
    return;
  }
  int q = FixedQ > 0 ? FixedQ : (1 << (h - 2));
  int blocks = FixedQ > 0 ? (n / (FixedQ << 2)) : (n >> h);
  for (int s = 0; s < blocks; ++s) {
    u32 iw2 = mmul<lazy>(iw1, iw1, info.mont);
    u32 iw3 = mmul<lazy>(iw1, iw2, info.mont);
    u32* p0 = a + (FixedQ > 0 ? s * (FixedQ << 2) : (s << h));
    u32 *p1 = p0 + q, *p2 = p1 + q, *p3 = p2 + q;
    for (int i = 0; i < q; ++i) {
      u32 c0 = p0[i], c1 = p1[i], c2 = p2[i], c3 = p3[i];
      u32 u0 = add_lim(c0, c1, lim), u1 = sub_lim(c0, c1, lim);
      u32 u2 = add_lim(c2, c3, lim);
      u32 u3 = mmul<lazy>(c2 + lim - c3, info.iimag, info.mont);
      p0[i] = add_lim(u0, u2, lim);
      p2[i] = mmul<lazy>(u0 + lim - u2, iw2, info.mont);
      p1[i] = mmul<lazy>(u1 + u3, iw1, info.mont);
      p3[i] = mmul<lazy>(u1 + lim - u3, iw3, info.mont);
    }
    if (s + 1 < blocks) {
      iw1 = mmul<lazy>(iw1, info.istep[1][std::countr_one((u32)s)], info.mont);
    }
  }
}

template <int m, bool HalfZero = false> void butterfly(u32* a, int n) {
  const auto& info = ntt_info<m>;
  constexpr u32 mod = NttInfo<m>::mod;
  constexpr bool lazy = NttInfo<m>::lazy;
  constexpr u32 lim = lazy ? mod * 2U : mod;
  int h = std::countr_zero((u32)n);

  if (h & 1) {
    int p = n >> 1;
    if constexpr (HalfZero) {
      std::memcpy(a + p, a, sizeof(u32) * (u32)p);
    } else {
      for (int i = 0; i < p; ++i) {
        u32 x = a[i], y = a[i + p];
        a[i] = add_lim(x, y, lim);
        a[i + p] = sub_lim(x, y, lim);
      }
    }
    --h;
  } else if constexpr (HalfZero) {
    if (n >= 4) {
      int q = n >> 2;
      u32 *p0 = a, *p1 = a + q, *p2 = p1 + q, *p3 = p2 + q;
      for (int i = 0; i < q; ++i) {
        u32 a0 = p0[i], a1 = p1[i];
        u32 u3 = mmul<lazy>(a1, info.imag, info.mont);
        p0[i] = add_lim(a0, a1, lim);
        p1[i] = sub_lim(a0, a1, lim);
        p2[i] = add_lim(a0, u3, lim);
        p3[i] = sub_lim(a0, u3, lim);
      }
      h -= 2;
    }
  }

  if (h <= 12) {
    for (; h >= 6; h -= 2) r4_fwd<m>(a, n, h, info.mont.one());
    if (h >= 4) r4_fwd<m, 4>(a, n, 4, info.mont.one());
    if (h >= 2) r4_fwd<m, 1>(a, n, 2, info.mont.one());
    return;
  }

  for (; h > 12; h -= 2) r4_fwd<m>(a, n, h, info.mont.one());

  u32 one = info.mont.one();
  u32 w_init[6] = {one, one, one, one, one, one};
  int s_end = n >> 12;
  for (int s = 0; s < s_end; ++s) {
    u32* blk = a + (s << 12);
    r4_fwd<m>(blk, 1 << 12, 12, w_init[0]);
    r4_fwd<m>(blk, 1 << 12, 10, w_init[1]);
    r4_fwd<m>(blk, 1 << 12, 8, w_init[2]);
    r4_fwd<m>(blk, 1 << 12, 6, w_init[3]);
    r4_fwd<m, 4>(blk, 1 << 12, 4, w_init[4]);
    r4_fwd<m, 1>(blk, 1 << 12, 2, w_init[5]);
    if (s + 1 < s_end) {
      int k = std::countr_one((u32)s);
      for (int j = 0; j < 6; ++j) {
        w_init[j] = mmul<lazy>(w_init[j], info.step[2 * j + 1][k], info.mont);
      }
    }
  }
}

template <int m, bool SameInput>
inline void pw_mul(u32* a, const u32* b, int n) {
  for (int i = 0; i < n; ++i) {
    a[i] =
        mmul<NttInfo<m>::lazy>(a[i], SameInput ? a[i] : b[i], ntt_info<m>.mont);
  }
}

template <int m, bool SameInput>
void butterfly_inv(u32* a, const u32* b, int n) {
  const auto& info = ntt_info<m>;
  constexpr u32 mod = NttInfo<m>::mod;
  constexpr bool lazy = NttInfo<m>::lazy;
  constexpr u32 lim = lazy ? mod * 2U : mod;
  int log_n = std::countr_zero((u32)n);

  int h = 2;
  if (log_n <= 12) {
    pw_mul<m, SameInput>(a, b, n);
    if (2 <= log_n) r4_inv<m, 1>(a, n, 2, info.mont.one());
    if (4 <= log_n) r4_inv<m, 4>(a, n, 4, info.mont.one());
    for (h = 6; h <= log_n; h += 2) r4_inv<m>(a, n, h, info.mont.one());
    if (log_n & 1) {
      int p = n >> 1;
      for (int i = 0; i < p; ++i) {
        u32 x = a[i], y = a[i + p];
        a[i] = add_lim(x, y, lim);
        a[i + p] = sub_lim(x, y, lim);
      }
    }
    return;
  }

  u32 one = info.mont.one();
  u32 iw_init[6] = {one, one, one, one, one, one};
  int s_end = n >> 12;
  for (int s = 0; s < s_end; ++s) {
    int blk_off = s << 12;
    u32* blk = a + blk_off;
    pw_mul<m, SameInput>(blk, SameInput ? nullptr : b + blk_off, 1 << 12);
    r4_inv<m, 1>(blk, 1 << 12, 2, iw_init[5]);
    r4_inv<m, 4>(blk, 1 << 12, 4, iw_init[4]);
    r4_inv<m>(blk, 1 << 12, 6, iw_init[3]);
    r4_inv<m>(blk, 1 << 12, 8, iw_init[2]);
    r4_inv<m>(blk, 1 << 12, 10, iw_init[1]);
    r4_inv<m>(blk, 1 << 12, 12, iw_init[0]);
    if (s + 1 < s_end) {
      int k = std::countr_one((u32)s);
      for (int j = 0; j < 6; ++j) {
        iw_init[j] =
            mmul<lazy>(iw_init[j], info.istep[2 * j + 1][k], info.mont);
      }
    }
  }

  for (h = 14; h <= log_n; h += 2) r4_inv<m>(a, n, h, info.mont.one());
  if (log_n & 1) {
    int p = n >> 1;
    for (int i = 0; i < p; ++i) {
      u32 x = a[i], y = a[i + p];
      a[i] = add_lim(x, y, lim);
      a[i + p] = sub_lim(x, y, lim);
    }
  }
}

template <int m, typename OutPtr, typename Transform>
void scale_out_impl(const u32* src, OutPtr dst, int n, u32 factor,
                    Transform&& transform) {
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
    internal_ntt::butterfly_inv<m, true>(a, nullptr, z);
  } else {
    internal_ntt::butterfly_inv<m, false>(a, b, z);
  }

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
