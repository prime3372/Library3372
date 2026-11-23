// Repository: Library3372
// File: benchmarks/math/convolution_mod_1000000007/solutions/radix2_crt.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/convolution_mod_1000000007/solutions/radix2_crt.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <bit>
#include <iostream>
#include <utility>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"

namespace {

unsigned int mod_pow(unsigned int base, unsigned int exp, unsigned int mod) {
  unsigned long long res = 1;
  unsigned long long b = base;
  while (exp > 0) {
    if (exp & 1U) res = res * b % mod;
    b = b * b % mod;
    exp >>= 1;
  }
  return (unsigned int)res;
}

void ntt_radix2(std::vector<unsigned int>& a, bool invert, unsigned int mod,
                unsigned int root) {
  int n = (int)a.size();
  for (int i = 1, j = 0; i < n; ++i) {
    int bit = n >> 1;
    for (; j & bit; bit >>= 1) j ^= bit;
    j ^= bit;
    if (i < j) std::swap(a[i], a[j]);
  }

  unsigned int g = invert ? mod_pow(root, mod - 2, mod) : root;
  for (int len = 2; len <= n; len <<= 1) {
    unsigned int wlen = mod_pow(g, (mod - 1) / (unsigned int)len, mod);
    int half = len >> 1;
    for (int i = 0; i < n; i += len) {
      unsigned int w = 1;
      for (int j = 0; j < half; ++j) {
        unsigned int u = a[i + j];
        unsigned int v =
            (unsigned int)((unsigned long long)a[i + j + half] * w % mod);
        unsigned int s = u + v;
        a[i + j] = s >= mod ? s - mod : s;
        a[i + j + half] = u >= v ? u - v : u + mod - v;
        w = (unsigned int)((unsigned long long)w * wlen % mod);
      }
    }
  }

  if (invert) {
    unsigned int inv_n = mod_pow((unsigned int)n, mod - 2, mod);
    for (int i = 0; i < n; ++i) {
      a[i] = (unsigned int)((unsigned long long)a[i] * inv_n % mod);
    }
  }
}

std::vector<unsigned int> conv_prime(const std::vector<unsigned int>& a,
                                     const std::vector<unsigned int>& b, int z,
                                     int s, unsigned int mod,
                                     unsigned int root) {
  std::vector<unsigned int> fa(z, 0), fb(z, 0);
  for (int i = 0; i < (int)a.size(); ++i) fa[i] = a[i] % mod;
  for (int i = 0; i < (int)b.size(); ++i) fb[i] = b[i] % mod;
  ntt_radix2(fa, false, mod, root);
  ntt_radix2(fb, false, mod, root);
  for (int i = 0; i < z; ++i) {
    fa[i] = (unsigned int)((unsigned long long)fa[i] * fb[i] % mod);
  }
  ntt_radix2(fa, true, mod, root);
  fa.resize(s);
  return fa;
}

}  // namespace

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n, m;
  std::cin >> n >> m;
  std::vector<unsigned int> a(n), b(m);
  for (int i = 0; i < n; ++i) std::cin >> a[i];
  for (int i = 0; i < m; ++i) std::cin >> b[i];

  library3372::internal::Timer timer;
  int s = n + m - 1;
  int z = (int)std::bit_ceil((unsigned int)s);
  constexpr unsigned int m0 = 167772161U;
  constexpr unsigned int m1 = 469762049U;
  constexpr unsigned int m2 = 754974721U;
  constexpr unsigned long long kOutMod = 1000000007ULL;

  std::vector<unsigned int> c0 = conv_prime(a, b, z, s, m0, 3U);
  std::vector<unsigned int> c1 = conv_prime(a, b, z, s, m1, 3U);
  std::vector<unsigned int> c2 = conv_prime(a, b, z, s, m2, 11U);

  unsigned int inv01 = mod_pow(m0 % m1, m1 - 2, m1);
  unsigned int m01_mod2 = (unsigned int)((unsigned long long)m0 * m1 % m2);
  unsigned int inv012 = mod_pow(m01_mod2, m2 - 2, m2);
  unsigned long long m01_out = (unsigned long long)m0 * m1 % kOutMod;

  std::vector<unsigned int> c(s);
  for (int i = 0; i < s; ++i) {
    unsigned int r0 = c0[i];
    unsigned int r1 = c1[i];
    unsigned int r2 = c2[i];
    unsigned int d1 = r1 >= r0 ? r1 - r0 : r1 + m1 - r0;
    unsigned int x1 = (unsigned int)((unsigned long long)d1 * inv01 % m1);
    unsigned long long v01 = r0 + (unsigned long long)m0 * x1;
    unsigned int v01_mod2 = (unsigned int)(v01 % m2);
    unsigned int d2 = r2 >= v01_mod2 ? r2 - v01_mod2 : r2 + m2 - v01_mod2;
    unsigned int x2 = (unsigned int)((unsigned long long)d2 * inv012 % m2);
    c[i] = (unsigned int)((v01 + m01_out * x2) % kOutMod);
  }
  double run_sec = timer.elapsed();

  unsigned long long ans_hash = 0;
  for (unsigned int x : c) {
    ans_hash =
        library3372::internal::hash_combine(ans_hash, (unsigned long long)x);
  }

  std::cout << "hash " << ans_hash << '\n';
  std::cout << "time run " << run_sec << '\n';
  return 0;
}
