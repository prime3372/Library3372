// Repository: Library3372
// File: library3372/internal/math/prime_sieve.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/internal/math/prime_sieve.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_INTERNAL_MATH_PRIME_SIEVE_HPP_
#define LIBRARY3372_LIBRARY3372_INTERNAL_MATH_PRIME_SIEVE_HPP_

#include <algorithm>
#include <array>
#include <bit>
#include <cstring>
#include <type_traits>
#include <vector>

namespace library3372 {
namespace internal_prime_sieve {

inline constexpr unsigned int kWheel[8] = {1, 7, 11, 13, 17, 19, 23, 29};
inline constexpr unsigned int kWheelDiff[8] = {6, 4, 2, 4, 2, 4, 6, 2};
inline constexpr unsigned int kPatternLen = 7U * 11U * 13U;
inline constexpr unsigned int kSegBytes = 32U * kPatternLen;
inline constexpr unsigned int kSmallPrimeThreshold = 16016U;

constexpr std::array<int, 30> make_wheel_idx() {
  std::array<int, 30> res{};
  res.fill(-1);
  for (int i = 0; i < 8; ++i) res[kWheel[i]] = i;
  return res;
}

constexpr std::array<unsigned int, 30> make_next_wheel() {
  std::array<unsigned int, 30> res{};
  int idx = 0;
  for (unsigned int r = 0; r < 30; ++r) {
    if (kWheel[idx] < r) ++idx;
    res[r] = (unsigned int)idx;
  }
  return res;
}

constexpr std::array<unsigned int, 64> make_word_offset() {
  std::array<unsigned int, 64> res{};
  for (int i = 0; i < 64; ++i) {
    res[i] = 30U * (unsigned int)(i >> 3) + kWheel[i & 7];
  }
  return res;
}

constexpr std::array<std::array<unsigned char, 8>, 8> make_out_mask() {
  const auto idx = make_wheel_idx();
  std::array<std::array<unsigned char, 8>, 8> res{};
  for (int rp = 0; rp < 8; ++rp) {
    for (int rm = 0; rm < 8; ++rm) {
      int r_out = idx[(kWheel[rp] * kWheel[rm]) % 30U];
      res[rp][rm] = (unsigned char)(~(1U << r_out));
    }
  }
  return res;
}

constexpr std::array<std::array<unsigned int, 8>, 8> make_carry() {
  const auto idx = make_wheel_idx();
  std::array<std::array<unsigned int, 8>, 8> res{};
  for (int rp = 0; rp < 8; ++rp) {
    for (int rm = 0; rm < 8; ++rm) {
      unsigned int r_out = kWheel[idx[(kWheel[rp] * kWheel[rm]) % 30U]];
      res[rp][rm] = (r_out + kWheel[rp] * kWheelDiff[rm]) / 30U;
    }
  }
  return res;
}

constexpr std::array<unsigned char, 30> make_first_byte_mask() {
  std::array<unsigned char, 30> res{};
  for (unsigned int rem = 0; rem < 30; ++rem) {
    unsigned char mask = 0;
    for (int r = 0; r < 8; ++r) {
      if (kWheel[r] >= rem) mask |= (unsigned char)(1U << r);
    }
    res[rem] = mask;
  }
  return res;
}

constexpr std::array<unsigned char, 30> make_last_byte_mask() {
  std::array<unsigned char, 30> res{};
  for (unsigned int rem = 0; rem < 30; ++rem) {
    unsigned char mask = 0;
    for (int r = 0; r < 8; ++r) {
      if (kWheel[r] <= rem) mask |= (unsigned char)(1U << r);
    }
    res[rem] = mask;
  }
  return res;
}

constexpr std::array<unsigned char, kPatternLen> make_pattern() {
  const auto idx = make_wheel_idx();
  std::array<unsigned char, kPatternLen> pat{};
  pat.fill(0xFF);
  for (unsigned int p : {7U, 11U, 13U}) {
    for (unsigned int m = 1;; m += 2) {
      if (m % 3 == 0 || m % 5 == 0) continue;
      unsigned int prod = p * m;
      unsigned int b = prod / 30U;
      if (b >= kPatternLen) break;
      pat[b] &= (unsigned char)(~(1U << idx[prod % 30U]));
    }
  }
  return pat;
}

inline constexpr auto kWheelIdx = make_wheel_idx();
inline constexpr auto kNextWheel = make_next_wheel();
inline constexpr auto kWordOffset = make_word_offset();
inline constexpr auto kOutMask = make_out_mask();
inline constexpr auto kCarry = make_carry();
inline constexpr auto kFirstByteMask = make_first_byte_mask();
inline constexpr auto kLastByteMask = make_last_byte_mask();
inline constexpr auto kPattern = make_pattern();

struct SmallSievePrime {
  unsigned int p;
  unsigned int pos[8];
  unsigned char mask[8];
};

struct LargeSievePrime {
  unsigned int qp;
  unsigned char rp;
  unsigned char rm;
  unsigned long long pos;
};

struct PushU32 {
  std::vector<unsigned int>& out;
  constexpr void operator()(unsigned long long p) const {
    out.push_back((unsigned int)p);
  }
};

}  // namespace internal_prime_sieve

namespace internal {

constexpr unsigned int isqrt_u64(unsigned long long x) {
  if (x == 0) return 0;
  unsigned long long r = 1ULL << ((std::bit_width(x) + 1) >> 1);
  while (true) {
    unsigned long long nr = (r + x / r) >> 1;
    if (nr >= r) return (unsigned int)r;
    r = nr;
  }
}

constexpr unsigned int estimate_prime_count(unsigned long long len,
                                            unsigned long long upper) {
  if (len <= 100ULL) return (unsigned int)len;
  int bw = std::bit_width(upper);
  int denom = bw * 693 - 1000;
  if (denom <= 0) denom = 1;
  unsigned long long est = (len * 1100ULL) / (unsigned long long)denom;
  if (est > len) est = len;
  return (unsigned int)est;
}

// Segmented Sieve of Eratosthenes with mod-30 wheel factorization
// https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes#Segmented_sieve
template <typename Emit>
constexpr void sieve_primes_wheel30(unsigned long long ul,
                                    unsigned long long ur,
                                    unsigned int sieve_limit, Emit&& emit) {
  using namespace internal_prime_sieve;
  std::vector<SmallSievePrime> small_primes;
  std::vector<LargeSievePrime> large_primes;
  const unsigned long long start_byte = ul / 30ULL;
  const unsigned long long end_byte = (ur - 1ULL) / 30ULL + 1ULL;
  const unsigned long long start_val = start_byte * 30ULL;

  if (sieve_limit >= 17U) {
    std::vector<unsigned int> base_primes;
    base_primes.reserve(estimate_prime_count(sieve_limit, sieve_limit));
    sieve_primes_wheel30(17ULL, (unsigned long long)sieve_limit + 1ULL,
                         isqrt_u64(sieve_limit), PushU32{base_primes});
    for (unsigned int p : base_primes) {
      const unsigned int rp = (unsigned int)kWheelIdx[p % 30U];
      unsigned long long m_min = (start_val + p - 1ULL) / p;
      if (m_min < p) m_min = p;
      const unsigned long long q_min = m_min / 30ULL;
      const unsigned int rm_start = kNextWheel[m_min % 30U];

      if (p < kSmallPrimeThreshold) {
        SmallSievePrime sp{};
        sp.p = p;
        for (unsigned int rm = 0; rm < 8U; ++rm) {
          unsigned long long q = q_min + (rm < rm_start ? 1ULL : 0ULL);
          unsigned long long m = 30ULL * q + kWheel[rm];
          unsigned long long prod = (unsigned long long)p * m;
          sp.pos[rm] = (unsigned int)(prod / 30ULL - start_byte);
          sp.mask[rm] = kOutMask[rp][rm];
        }
        small_primes.push_back(sp);
      } else {
        unsigned long long m = 30ULL * q_min + kWheel[rm_start];
        unsigned long long prod = (unsigned long long)p * m;
        large_primes.push_back(LargeSievePrime{p / 30U, (unsigned char)rp,
                                               (unsigned char)rm_start,
                                               prod / 30ULL - start_byte});
      }
    }
  }

  const unsigned int buf_bytes = (unsigned int)std::min<unsigned long long>(
      end_byte - start_byte, kSegBytes);
  const unsigned int buf_padded = (buf_bytes + 7U) & ~7U;
  std::vector<unsigned char> seg(buf_padded);

  for (unsigned long long cur_byte = start_byte; cur_byte < end_byte;) {
    const unsigned int seg_bytes = (unsigned int)std::min<unsigned long long>(
        end_byte - cur_byte, kSegBytes);
    const unsigned int seg_padded = (seg_bytes + 7U) & ~7U;

    unsigned int pat_off = (unsigned int)(cur_byte % kPatternLen);
    unsigned int dst = 0;
    while (dst < seg_bytes) {
      unsigned int chunk = std::min(seg_bytes - dst, kPatternLen - pat_off);
      std::copy_n(kPattern.data() + pat_off, chunk, seg.data() + dst);
      dst += chunk;
      pat_off = 0;
    }
    if (cur_byte == 0ULL) seg[0] = 0xFE;

    for (SmallSievePrime& sp : small_primes) {
      for (int rm = 0; rm < 8; ++rm) {
        unsigned int idx = sp.pos[rm];
        const unsigned char m = sp.mask[rm];
        for (; idx < seg_bytes; idx += sp.p) seg[idx] &= m;
        sp.pos[rm] = idx - seg_bytes;
      }
    }

    for (LargeSievePrime& lp : large_primes) {
      if (lp.pos >= seg_bytes) {
        lp.pos -= seg_bytes;
        continue;
      }
      unsigned int idx = (unsigned int)lp.pos;
      unsigned int rm = lp.rm;
      const unsigned int qp = lp.qp;
      const unsigned int rp = lp.rp;
      while (idx < seg_bytes) {
        seg[idx] &= kOutMask[rp][rm];
        idx += qp * kWheelDiff[rm] + kCarry[rp][rm];
        rm = (rm + 1U) & 7U;
      }
      lp.pos = (unsigned long long)(idx - seg_bytes);
      lp.rm = (unsigned char)rm;
    }

    if (cur_byte == start_byte) seg[0] &= kFirstByteMask[ul % 30ULL];
    if (cur_byte + seg_bytes == end_byte) {
      seg[seg_bytes - 1] &= kLastByteMask[(ur - 1ULL) % 30ULL];
    }
    for (unsigned int i = seg_bytes; i < seg_padded; ++i) seg[i] = 0;

    const unsigned long long base_val = cur_byte * 30ULL;
    if (std::is_constant_evaluated()) {
      for (unsigned int i = 0; i < seg_bytes; ++i) {
        unsigned int b = seg[i];
        while (b != 0U) {
          int bit = std::countr_zero(b);
          b &= b - 1U;
          emit(base_val + 30ULL * i + kWheel[bit]);
        }
      }
    } else {
      for (unsigned int i = 0; i < seg_bytes; i += 8U) {
        unsigned long long w = 0;
        std::memcpy(&w, seg.data() + i, 8);
        const unsigned long long word_base = base_val + 30ULL * i;
        while (w != 0ULL) {
          int bit = std::countr_zero(w);
          w &= w - 1ULL;
          emit(word_base + kWordOffset[bit]);
        }
      }
    }

    cur_byte += seg_bytes;
  }
}

}  // namespace internal
}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_INTERNAL_MATH_PRIME_SIEVE_HPP_
