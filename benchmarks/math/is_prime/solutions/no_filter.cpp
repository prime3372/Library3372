// Repository: Library3372
// File: benchmarks/math/is_prime/solutions/no_filter.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/is_prime/solutions/no_filter.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <bit>
#include <iostream>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"
#include "library3372/internal/math/montgomery.hpp"

namespace {

constexpr bool is_prime_no_filter(long long n) {
  if (n <= 1) return false;
  if (n == 2) return true;
  if ((n & 1) == 0) return false;

  const unsigned long long u = (unsigned long long)n;
  unsigned long long d = u - 1;
  const int s = std::countr_zero(d);
  d >>= s;

  const library3372::internal::Montgomery64 mont(u);
  const unsigned long long one = mont.one();
  const unsigned long long neg_one = u - one;

  auto check_bases = [&](const unsigned long long* bases, int len) -> bool {
    for (int i = 0; i < len; ++i) {
      unsigned long long x = mont.to_mont(bases[i]);
      if (x == 0) continue;
      x = mont.pow(x, d);
      if (x == one || x == neg_one) continue;
      for (int r = 1; r < s; ++r) {
        x = mont.mul(x, x);
        if (x == neg_one) break;
      }
      if (x != neg_one) return false;
    }
    return true;
  };

  if (u < 4759123141ULL) {
    constexpr unsigned long long bases32[] = {2, 7, 61};
    return check_bases(bases32, 3);
  }
  constexpr unsigned long long bases64[] = {2,      325,     9375,      28178,
                                            450775, 9780504, 1795265022};
  return check_bases(bases64, 7);
}

}  // namespace

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int q;
  std::cin >> q;
  std::vector<long long> a(q);
  for (int i = 0; i < q; ++i) std::cin >> a[i];

  library3372::internal::Timer timer;
  std::vector<unsigned long long> res(q);
  for (int i = 0; i < q; ++i) res[i] = is_prime_no_filter(a[i]) ? 1ULL : 0ULL;
  double run_sec = timer.elapsed();

  unsigned long long ans_hash = 0;
  for (int i = 0; i < q; ++i) {
    ans_hash = library3372::internal::hash_combine(ans_hash, res[i]);
  }

  std::cout << "hash " << ans_hash << '\n';
  std::cout << "time run " << run_sec << '\n';
  return 0;
}
