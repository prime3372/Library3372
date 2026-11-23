// Repository: Library3372
// File: benchmarks/math/convolution_mod/solutions/radix2_ntt.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/convolution_mod/solutions/radix2_ntt.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <bit>
#include <iostream>
#include <utility>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"

namespace {

constexpr unsigned int kMod = 998244353U;
constexpr unsigned int kRoot = 3U;

unsigned int mod_pow(unsigned int base, unsigned int exp) {
  unsigned long long res = 1;
  unsigned long long b = base;
  while (exp > 0) {
    if (exp & 1U) res = res * b % kMod;
    b = b * b % kMod;
    exp >>= 1;
  }
  return (unsigned int)res;
}

void ntt_radix2(std::vector<unsigned int>& a, bool invert) {
  int n = (int)a.size();
  for (int i = 1, j = 0; i < n; ++i) {
    int bit = n >> 1;
    for (; j & bit; bit >>= 1) j ^= bit;
    j ^= bit;
    if (i < j) std::swap(a[i], a[j]);
  }

  unsigned int g = invert ? mod_pow(kRoot, kMod - 2) : kRoot;
  for (int len = 2; len <= n; len <<= 1) {
    unsigned int wlen = mod_pow(g, (kMod - 1) / (unsigned int)len);
    int half = len >> 1;
    for (int i = 0; i < n; i += len) {
      unsigned int w = 1;
      for (int j = 0; j < half; ++j) {
        unsigned int u = a[i + j];
        unsigned int v =
            (unsigned int)((unsigned long long)a[i + j + half] * w % kMod);
        unsigned int s = u + v;
        a[i + j] = s >= kMod ? s - kMod : s;
        a[i + j + half] = u >= v ? u - v : u + kMod - v;
        w = (unsigned int)((unsigned long long)w * wlen % kMod);
      }
    }
  }

  if (invert) {
    unsigned int inv_n = mod_pow((unsigned int)n, kMod - 2);
    for (int i = 0; i < n; ++i) {
      a[i] = (unsigned int)((unsigned long long)a[i] * inv_n % kMod);
    }
  }
}

}  // namespace

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n, m;
  std::cin >> n >> m;
  int s = n + m - 1;
  int z = (int)std::bit_ceil((unsigned int)s);
  std::vector<unsigned int> a(z, 0), b(z, 0);
  for (int i = 0; i < n; ++i) std::cin >> a[i];
  for (int i = 0; i < m; ++i) std::cin >> b[i];

  library3372::internal::Timer timer;
  ntt_radix2(a, false);
  ntt_radix2(b, false);
  for (int i = 0; i < z; ++i) {
    a[i] = (unsigned int)((unsigned long long)a[i] * b[i] % kMod);
  }
  ntt_radix2(a, true);
  a.resize(s);
  double run_sec = timer.elapsed();

  unsigned long long ans_hash = 0;
  for (unsigned int x : a) {
    ans_hash =
        library3372::internal::hash_combine(ans_hash, (unsigned long long)x);
  }

  std::cout << "hash " << ans_hash << '\n';
  std::cout << "time run " << run_sec << '\n';
  return 0;
}
