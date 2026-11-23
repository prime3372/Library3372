// Repository: Library3372
// File: benchmarks/string/suffix_array/solutions/doubling.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/string/suffix_array/solutions/doubling.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <algorithm>
#include <iostream>
#include <numeric>
#include <utility>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n;
  std::cin >> n;
  std::vector<int> a(n);
  for (int i = 0; i < n; ++i) std::cin >> a[i];

  library3372::internal::Timer timer;
  std::vector<int> sa(n);
  std::vector<int> rnk = a;
  std::vector<int> tmp(n);
  std::iota(sa.begin(), sa.end(), 0);
  for (int k = 1; k < n; k *= 2) {
    auto cmp = [&](int x, int y) {
      if (rnk[x] != rnk[y]) return rnk[x] < rnk[y];
      int rx = x + k < n ? rnk[x + k] : -1;
      int ry = y + k < n ? rnk[y + k] : -1;
      return rx < ry;
    };
    std::sort(sa.begin(), sa.end(), cmp);
    tmp[sa[0]] = 0;
    for (int i = 1; i < n; ++i) {
      tmp[sa[i]] = tmp[sa[i - 1]] + (cmp(sa[i - 1], sa[i]) ? 1 : 0);
    }
    std::swap(tmp, rnk);
    if (rnk[sa[n - 1]] == n - 1) break;
  }
  double build_sec = timer.elapsed();

  unsigned long long ans_hash = 0ULL;
  for (int x : sa) {
    ans_hash =
        library3372::internal::hash_combine(ans_hash, (unsigned long long)x);
  }

  std::cout << "hash " << ans_hash << "\n";
  std::cout << "time build " << build_sec << "\n";
  return 0;
}
