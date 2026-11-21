// Repository: Library3372
// File: benchmarks/math/static_modint_raw/solutions/constructor.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/static_modint_raw/solutions/constructor.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <iostream>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"
#include "library3372/math/static_modint.hpp"

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n, k;
  std::cin >> n >> k;
  std::vector<int> a(n);
  for (int i = 0; i < n; ++i) std::cin >> a[i];

  using Mint = library3372::StaticModInt<998244353>;

  library3372::internal::Timer timer;
  std::vector<Mint> c(n);
  for (int r = 0; r < k; ++r) {
    int s = r % n;
    for (int i = 0; i < n - s; ++i) c[i] += Mint(a[i + s]);
    for (int i = n - s; i < n; ++i) c[i] += Mint(a[i + s - n]);
  }
  double run_sec = timer.elapsed();

  unsigned long long ans_hash = 0;
  for (int i = 0; i < n; ++i) {
    ans_hash = library3372::internal::hash_combine(
        ans_hash, (unsigned long long)c[i].val());
  }

  std::cout << "hash " << ans_hash << '\n';
  std::cout << "time run " << run_sec << '\n';
  return 0;
}
