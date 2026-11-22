// Repository: Library3372
// File: benchmarks/string/suffix_array/solutions/naive.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/string/suffix_array/solutions/naive.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <algorithm>
#include <iostream>
#include <numeric>
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

  if (n > 3000) {
    std::cout << "skip N > 3000\n";
    return 0;
  }

  library3372::internal::Timer timer;
  std::vector<int> sa(n);
  std::iota(sa.begin(), sa.end(), 0);
  std::sort(sa.begin(), sa.end(), [&](int l, int r) {
    int limit = n - (l > r ? l : r);
    for (int k = 0; k < limit; ++k) {
      if (a[l + k] != a[r + k]) return a[l + k] < a[r + k];
    }
    return l > r;
  });
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
