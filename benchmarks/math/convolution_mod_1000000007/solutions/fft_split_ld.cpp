// Repository: Library3372
// File: benchmarks/math/convolution_mod_1000000007/solutions/fft_split_ld.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/convolution_mod_1000000007/solutions/fft_split_ld.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <bit>
#include <cmath>
#include <iostream>
#include <utility>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"

namespace {

struct Cpx {
  long double x = 0;
  long double y = 0;
  Cpx operator+(const Cpx& o) const { return {x + o.x, y + o.y}; }
  Cpx operator-(const Cpx& o) const { return {x - o.x, y - o.y}; }
  Cpx operator*(const Cpx& o) const {
    return {x * o.x - y * o.y, x * o.y + y * o.x};
  }
};

void fft_ld(std::vector<Cpx>& a, bool invert) {
  int n = (int)a.size();
  for (int i = 1, j = 0; i < n; ++i) {
    int bit = n >> 1;
    for (; j & bit; bit >>= 1) j ^= bit;
    j ^= bit;
    if (i < j) std::swap(a[i], a[j]);
  }

  static const long double kPi = std::acos(-1.0L);
  std::vector<Cpx> roots(n);
  roots[0] = {1.0L, 0.0L};
  for (int half = 1; half < n; half <<= 1) {
    long double ang = (invert ? -kPi : kPi) / (long double)half;
    for (int j = 0; j < half; ++j) {
      roots[half + j] = {std::cos(ang * (long double)j),
                         std::sin(ang * (long double)j)};
    }
  }

  for (int half = 1; half < n; half <<= 1) {
    int len = half << 1;
    for (int i = 0; i < n; i += len) {
      for (int j = 0; j < half; ++j) {
        Cpx u = a[i + j];
        Cpx v = a[i + j + half] * roots[half + j];
        a[i + j] = u + v;
        a[i + j + half] = u - v;
      }
    }
  }
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

  std::vector<Cpx> fa(z), fb(z);
  for (int i = 0; i < n; ++i) {
    fa[i] = {(long double)(a[i] & 32767U), (long double)(a[i] >> 15)};
  }
  for (int i = 0; i < m; ++i) {
    fb[i] = {(long double)(b[i] & 32767U), (long double)(b[i] >> 15)};
  }

  fft_ld(fa, false);
  fft_ld(fb, false);

  long double ratio = 0.5L / (long double)z;
  std::vector<Cpx> r(z), t(z);
  for (int i = 0; i < z; ++i) {
    int j = (z - i) & (z - 1);
    Cpx pi = fa[i];
    Cpx pj = fa[j];
    Cpx da0{(pi.x + pj.x) * ratio, (pi.y - pj.y) * ratio};
    Cpx da1{(pi.y + pj.y) * ratio, (pj.x - pi.x) * ratio};
    r[i] = da0 * fb[i];
    t[i] = da1 * fb[i];
  }

  fft_ld(r, true);
  fft_ld(t, true);

  constexpr unsigned long long kOutMod = 1000000007ULL;
  std::vector<unsigned int> c(s);
  for (int i = 0; i < s; ++i) {
    unsigned long long c00 = (unsigned long long)std::llround(r[i].x) % kOutMod;
    unsigned long long c01 =
        (unsigned long long)(std::llround(r[i].y) + std::llround(t[i].x)) %
        kOutMod;
    unsigned long long c11 = (unsigned long long)std::llround(t[i].y) % kOutMod;
    c[i] =
        (unsigned int)((c00 + (c01 << 15) + ((c11 << 30) % kOutMod)) % kOutMod);
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
