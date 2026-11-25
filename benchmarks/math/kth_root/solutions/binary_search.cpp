// Repository: Library3372
// File: benchmarks/math/kth_root/solutions/binary_search.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/kth_root/solutions/binary_search.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <bit>
#include <iostream>
#include <utility>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"

namespace {

bool pow_le(unsigned long long x, int k, unsigned long long u) {
  unsigned long long res = 1;
  while (true) {
    if (k & 1) {
      unsigned __int128 p = (unsigned __int128)res * x;
      if (p > u) return false;
      res = (unsigned long long)p;
    }
    k >>= 1;
    if (k == 0) return true;
    unsigned __int128 sq = (unsigned __int128)x * x;
    if (sq > u) return false;
    x = (unsigned long long)sq;
  }
}

long long kth_root_bsearch(long long n, long long k) {
  if (n <= 1 || k == 1) return n;
  unsigned long long u = (unsigned long long)n;
  int w = std::bit_width(u);
  if (k >= w) return 1;
  int ik = (int)k;
  int b = (w - 1) / ik;
  unsigned long long lo = 1ULL << b;
  unsigned long long hi = lo << 1;
  while (hi - lo > 1) {
    unsigned long long mid = (lo + hi) >> 1;
    if (pow_le(mid, ik, u)) {
      lo = mid;
    } else {
      hi = mid;
    }
  }
  return (long long)lo;
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
  std::vector<long long> res(q);
  for (int i = 0; i < q; ++i) {
    res[i] = kth_root_bsearch(qs[i].first, qs[i].second);
  }
  double run_sec = timer.elapsed();

  unsigned long long ans_hash = 0;
  for (int i = 0; i < q; ++i) {
    ans_hash = library3372::internal::hash_combine(ans_hash,
                                                   (unsigned long long)res[i]);
  }

  std::cout << "hash " << ans_hash << '\n';
  std::cout << "time run " << run_sec << '\n';
  return 0;
}
