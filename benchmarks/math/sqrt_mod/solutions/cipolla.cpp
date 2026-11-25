// Repository: Library3372
// File: benchmarks/math/sqrt_mod/solutions/cipolla.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/sqrt_mod/solutions/cipolla.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <bit>
#include <cassert>
#include <iostream>
#include <optional>
#include <utility>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"
#include "library3372/internal/math/montgomery.hpp"
#include "library3372/math/is_prime.hpp"

namespace {

std::optional<long long> sqrt_mod_cipolla(long long a, long long p) {
  assert(library3372::is_prime(p));
  long long rem = a % p;
  if (rem < 0) rem += p;
  if (rem <= 1) return rem;

  const unsigned long long u = (unsigned long long)p;
  const library3372::internal::Montgomery64 mont(u);
  const unsigned long long one = mont.one();
  const unsigned long long neg_one = u - one;
  const unsigned long long am = mont.to_mont((unsigned long long)rem);

  const unsigned long long half_p = (u - 1ULL) >> 1;
  if (mont.pow(am, half_p) != one) return std::nullopt;

  unsigned long long tm = 0;
  unsigned long long w = 0;
  for (;;) {
    w = mont.sub(mont.mul(tm, tm), am);
    if (mont.pow(w, half_p) == neg_one) break;
    tm = mont.add(tm, one);
  }

  const unsigned long long e = (u + 1ULL) >> 1;
  unsigned long long rx = tm;
  unsigned long long ry = one;
  for (unsigned long long bit = 1ULL << (std::bit_width(e) - 2); bit > 0;
       bit >>= 1) {
    const unsigned long long xx = mont.mul(rx, rx);
    const unsigned long long yy = mont.mul(ry, ry);
    const unsigned long long xy = mont.mul(rx, ry);
    rx = mont.add(xx, mont.mul(yy, w));
    ry = mont.add(xy, xy);
    if (e & bit) {
      const unsigned long long nx = mont.add(mont.mul(rx, tm), mont.mul(ry, w));
      ry = mont.add(rx, mont.mul(ry, tm));
      rx = nx;
    }
  }

  const long long res = (long long)mont.from_mont(rx);
  return res <= p - res ? res : p - res;
}

}  // namespace

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int q;
  std::cin >> q;
  std::vector<std::pair<long long, long long>> qs(q);
  for (int i = 0; i < q; ++i) std::cin >> qs[i].first >> qs[i].second;

  library3372::internal::Timer timer;
  std::vector<std::optional<long long>> res(q);
  for (int i = 0; i < q; ++i) {
    res[i] = sqrt_mod_cipolla(qs[i].first, qs[i].second);
  }
  double run_sec = timer.elapsed();

  unsigned long long ans_hash = 0;
  for (int i = 0; i < q; ++i) {
    unsigned long long v =
        res[i].has_value() ? (unsigned long long)(*res[i]) : ~0ULL;
    ans_hash = library3372::internal::hash_combine(ans_hash, v);
  }

  std::cout << "hash " << ans_hash << '\n';
  std::cout << "time run " << run_sec << '\n';
  return 0;
}
