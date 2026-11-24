// Repository: Library3372
// File: library3372/internal/math/ntt64.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/internal/math/ntt64.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_INTERNAL_MATH_NTT64_HPP_
#define LIBRARY3372_LIBRARY3372_INTERNAL_MATH_NTT64_HPP_

#include <bit>
#include <cstring>

#include "library3372/internal/math/montgomery.hpp"
#include "library3372/math/factorize.hpp"
#include "library3372/math/is_prime.hpp"

namespace library3372::internal {
namespace internal_ntt64 {

using u32 = unsigned int;
using i64 = long long;
using u64 = unsigned long long;

constexpr u64 ntt_primitive_root64(i64 m) {
  if (m == 2) return 1;
  auto factors = factorize(m - 1);
  Montgomery64 mont((u64)m);
  u64 one = mont.one();
  for (u64 g = 2;; ++g) {
    u64 gm = mont.to_mont(g);
    bool ok = true;
    for (const auto& [p, e] : factors) {
      if (mont.pow(gm, ((u64)m - 1ULL) / (u64)p) == one) {
        ok = false;
        break;
      }
    }
    if (ok) return g;
  }
}

}  // namespace internal_ntt64

template <long long m> struct NttInfo64 {
  using i64 = internal_ntt64::i64;
  using u64 = internal_ntt64::u64;
  static constexpr bool valid = (m >= 2 && is_prime(m));
  static constexpr u64 mod = (u64)(valid ? m : 3);
  static constexpr bool lazy = (mod < (1ULL << 62));
  static constexpr int rank2 = std::countr_zero(mod - 1);
  static constexpr Montgomery64 mont{mod == 2 ? 3ULL : mod};

  u64 imag = 0;
  u64 iimag = 0;
  u64 step[13][64] = {};
  u64 istep[13][64] = {};

  constexpr NttInfo64() {
    if constexpr (!valid || mod == 2) return;
    u64 g = internal_ntt64::ntt_primitive_root64((i64)mod);
    u64 g_mont = mont.to_mont(g);

    u64 root[64] = {};
    u64 iroot[64] = {};
    root[rank2] = mont.pow(g_mont, (mod - 1ULL) >> rank2);
    iroot[rank2] = mont.pow(root[rank2], mod - 2ULL);
    for (int i = rank2 - 1; i >= 0; --i) {
      root[i] = mont.mul(root[i + 1], root[i + 1]);
      iroot[i] = mont.mul(iroot[i + 1], iroot[i + 1]);
    }

    if (rank2 >= 2) {
      imag = root[2];
      iimag = iroot[2];
    }

    for (int d = 0; d < 13; ++d) {
      u64 prod = mont.one();
      u64 iprod = mont.one();
      for (int k = 0; k + d <= rank2 - 2; ++k) {
        step[d][k] = mont.mul(root[k + d + 2], prod);
        istep[d][k] = mont.mul(iroot[k + d + 2], iprod);
        prod = mont.mul(prod, iroot[k + d + 2]);
        iprod = mont.mul(iprod, root[k + d + 2]);
      }
    }
  }
};

template <long long m> inline constexpr NttInfo64<m> ntt_info64{};

namespace internal_ntt64 {

inline u64 add_lim(u64 a, u64 b, u64 lim) {
  u64 s = a + b;
  return s >= lim ? s - lim : s;
}

inline u64 sub_lim(u64 a, u64 b, u64 lim) {
  return a >= b ? a - b : a + lim - b;
}

template <bool Lazy> inline u64 mmul(u64 a, u64 b, const Montgomery64& mont) {
  if constexpr (Lazy) return mont.mul_2m(a, b);
  return mont.mul(a, b);
}

template <long long m, int FixedQ = 0>
inline void r4_fwd(u64* a, int n, int h, u64 w1) {
  const auto& info = ntt_info64<m>;
  constexpr bool lazy = NttInfo64<m>::lazy;
  constexpr u64 lim = lazy ? NttInfo64<m>::mod * 2ULL : NttInfo64<m>::mod;
  if constexpr (FixedQ == 1) {
    int blocks = n >> 2;
    auto bfly4 = [&](u64* p, u64 w1_s) {
      u64 w2_s = mmul<lazy>(w1_s, w1_s, info.mont);
      u64 w3_s = mmul<lazy>(w1_s, w2_s, info.mont);
      u64 a0 = p[0];
      u64 x1 = mmul<lazy>(p[1], w1_s, info.mont);
      u64 x2 = mmul<lazy>(p[2], w2_s, info.mont);
      u64 x3 = mmul<lazy>(p[3], w3_s, info.mont);
      u64 u0 = add_lim(a0, x2, lim), u2 = sub_lim(a0, x2, lim);
      u64 u1 = add_lim(x1, x3, lim);
      u64 u3 = mmul<lazy>(x1 + lim - x3, info.imag, info.mont);
      p[0] = add_lim(u0, u1, lim);
      p[1] = sub_lim(u0, u1, lim);
      p[2] = add_lim(u2, u3, lim);
      p[3] = sub_lim(u2, u3, lim);
    };
    if (blocks == 1) {
      bfly4(a, w1);
      return;
    }
    u64 w1_0 = w1, w1_1 = mmul<lazy>(w1, info.step[1][0], info.mont);
    int u_end = blocks >> 1;
    for (int u = 0; u < u_end; ++u) {
      u64* p = a + (u << 3);
      bfly4(p, w1_0);
      bfly4(p + 4, w1_1);
      if (u + 1 < u_end) {
        u64 st = info.step[2][std::countr_one((u32)u)];
        w1_0 = mmul<lazy>(w1_0, st, info.mont);
        w1_1 = mmul<lazy>(w1_1, st, info.mont);
      }
    }
    return;
  }
  int q = FixedQ > 0 ? FixedQ : (1 << (h - 2));
  int blocks = FixedQ > 0 ? (n / (FixedQ << 2)) : (n >> h);
  for (int s = 0; s < blocks; ++s) {
    u64 w2 = mmul<lazy>(w1, w1, info.mont);
    u64 w3 = mmul<lazy>(w1, w2, info.mont);
    u64* p0 = a + (FixedQ > 0 ? s * (FixedQ << 2) : (s << h));
    u64 *p1 = p0 + q, *p2 = p1 + q, *p3 = p2 + q;
    for (int i = 0; i < q; ++i) {
      u64 a0 = p0[i];
      u64 x1 = mmul<lazy>(p1[i], w1, info.mont);
      u64 x2 = mmul<lazy>(p2[i], w2, info.mont);
      u64 x3 = mmul<lazy>(p3[i], w3, info.mont);
      u64 u0 = add_lim(a0, x2, lim), u2 = sub_lim(a0, x2, lim);
      u64 u1 = add_lim(x1, x3, lim);
      u64 u3 = mmul<lazy>(x1 + lim - x3, info.imag, info.mont);
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

template <long long m, int FixedQ = 0>
inline void r4_inv(u64* a, int n, int h, u64 iw1) {
  const auto& info = ntt_info64<m>;
  constexpr bool lazy = NttInfo64<m>::lazy;
  constexpr u64 lim = lazy ? NttInfo64<m>::mod * 2ULL : NttInfo64<m>::mod;
  if constexpr (FixedQ == 1) {
    int blocks = n >> 2;
    auto bfly4_inv = [&](u64* p, u64 iw1_s) {
      u64 iw2_s = mmul<lazy>(iw1_s, iw1_s, info.mont);
      u64 iw3_s = mmul<lazy>(iw1_s, iw2_s, info.mont);
      u64 c0 = p[0], c1 = p[1], c2 = p[2], c3 = p[3];
      u64 u0 = add_lim(c0, c1, lim), u1 = sub_lim(c0, c1, lim);
      u64 u2 = add_lim(c2, c3, lim);
      u64 u3 = mmul<lazy>(c2 + lim - c3, info.iimag, info.mont);
      p[0] = add_lim(u0, u2, lim);
      p[2] = mmul<lazy>(u0 + lim - u2, iw2_s, info.mont);
      p[1] = mmul<lazy>(u1 + u3, iw1_s, info.mont);
      p[3] = mmul<lazy>(u1 + lim - u3, iw3_s, info.mont);
    };
    if (blocks == 1) {
      bfly4_inv(a, iw1);
      return;
    }
    u64 iw1_0 = iw1, iw1_1 = mmul<lazy>(iw1, info.istep[1][0], info.mont);
    int u_end = blocks >> 1;
    for (int u = 0; u < u_end; ++u) {
      u64* p = a + (u << 3);
      bfly4_inv(p, iw1_0);
      bfly4_inv(p + 4, iw1_1);
      if (u + 1 < u_end) {
        u64 st = info.istep[2][std::countr_one((u32)u)];
        iw1_0 = mmul<lazy>(iw1_0, st, info.mont);
        iw1_1 = mmul<lazy>(iw1_1, st, info.mont);
      }
    }
    return;
  }
  int q = FixedQ > 0 ? FixedQ : (1 << (h - 2));
  int blocks = FixedQ > 0 ? (n / (FixedQ << 2)) : (n >> h);
  for (int s = 0; s < blocks; ++s) {
    u64 iw2 = mmul<lazy>(iw1, iw1, info.mont);
    u64 iw3 = mmul<lazy>(iw1, iw2, info.mont);
    u64* p0 = a + (FixedQ > 0 ? s * (FixedQ << 2) : (s << h));
    u64 *p1 = p0 + q, *p2 = p1 + q, *p3 = p2 + q;
    for (int i = 0; i < q; ++i) {
      u64 c0 = p0[i], c1 = p1[i], c2 = p2[i], c3 = p3[i];
      u64 u0 = add_lim(c0, c1, lim), u1 = sub_lim(c0, c1, lim);
      u64 u2 = add_lim(c2, c3, lim);
      u64 u3 = mmul<lazy>(c2 + lim - c3, info.iimag, info.mont);
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

template <long long m, bool HalfZero = false> void butterfly64(u64* a, int n) {
  const auto& info = ntt_info64<m>;
  constexpr u64 mod = NttInfo64<m>::mod;
  constexpr bool lazy = NttInfo64<m>::lazy;
  constexpr u64 lim = lazy ? mod * 2ULL : mod;
  int h = std::countr_zero((u32)n);

  if (h & 1) {
    int p = n >> 1;
    if constexpr (HalfZero) {
      std::memcpy(a + p, a, sizeof(u64) * (u32)p);
    } else {
      for (int i = 0; i < p; ++i) {
        u64 x = a[i], y = a[i + p];
        a[i] = add_lim(x, y, lim);
        a[i + p] = sub_lim(x, y, lim);
      }
    }
    --h;
  } else if constexpr (HalfZero) {
    if (n >= 4) {
      int q = n >> 2;
      u64 *p0 = a, *p1 = a + q, *p2 = p1 + q, *p3 = p2 + q;
      for (int i = 0; i < q; ++i) {
        u64 a0 = p0[i], a1 = p1[i];
        u64 u3 = mmul<lazy>(a1, info.imag, info.mont);
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

  u64 one = info.mont.one();
  u64 w_init[6] = {one, one, one, one, one, one};
  int s_end = n >> 12;
  for (int s = 0; s < s_end; ++s) {
    u64* blk = a + (s << 12);
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

template <long long m, bool SameInput>
inline void pw_mul(u64* a, const u64* b, int n) {
  for (int i = 0; i < n; ++i) {
    a[i] = mmul<NttInfo64<m>::lazy>(a[i], SameInput ? a[i] : b[i],
                                    ntt_info64<m>.mont);
  }
}

template <long long m, bool SameInput>
void butterfly_inv64(u64* a, const u64* b, int n) {
  const auto& info = ntt_info64<m>;
  constexpr u64 mod = NttInfo64<m>::mod;
  constexpr bool lazy = NttInfo64<m>::lazy;
  constexpr u64 lim = lazy ? mod * 2ULL : mod;
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
        u64 x = a[i], y = a[i + p];
        a[i] = add_lim(x, y, lim);
        a[i + p] = sub_lim(x, y, lim);
      }
    }
    return;
  }

  u64 one = info.mont.one();
  u64 iw_init[6] = {one, one, one, one, one, one};
  int s_end = n >> 12;
  for (int s = 0; s < s_end; ++s) {
    int blk_off = s << 12;
    u64* blk = a + blk_off;
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
      u64 x = a[i], y = a[i + p];
      a[i] = add_lim(x, y, lim);
      a[i + p] = sub_lim(x, y, lim);
    }
  }
}

}  // namespace internal_ntt64

template <long long m>
void ntt_forward_prepared64(unsigned long long* dst, int n, int z) {
  int half = z >> 1;
  if (z >= 4 && n <= half) {
    if (n < half) {
      std::memset(dst + n, 0,
                  sizeof(unsigned long long) * (unsigned int)(half - n));
    }
    internal_ntt64::butterfly64<m, true>(dst, z);
  } else {
    if (n < z) {
      std::memset(dst + n, 0,
                  sizeof(unsigned long long) * (unsigned int)(z - n));
    }
    internal_ntt64::butterfly64<m, false>(dst, z);
  }
}

template <long long m>
unsigned long long finish_ntt_convolution64(unsigned long long* a,
                                            const unsigned long long* b, int z,
                                            bool same_input) {
  if (same_input) {
    internal_ntt64::butterfly_inv64<m, true>(a, nullptr, z);
  } else {
    internal_ntt64::butterfly_inv64<m, false>(a, b, z);
  }
  const auto& info = ntt_info64<m>;
  const auto& mont = info.mont;
  unsigned long long iz_mont =
      mont.pow(mont.to_mont((unsigned long long)z % info.mod), info.mod - 2ULL);
  return mont.mul(iz_mont, mont.r2());
}

template <long long m, typename Transform>
void ntt_scale_out64(const unsigned long long* src, int n,
                     unsigned long long factor, Transform&& transform) {
  const auto& mont = ntt_info64<m>.mont;
  for (int i = 0; i < n; ++i) transform(i, mont.mul(src[i], factor));
}

}  // namespace library3372::internal

#endif  // LIBRARY3372_LIBRARY3372_INTERNAL_MATH_NTT64_HPP_
