// Repository: Library3372
// File: benchmarks/math/convolution_large/solutions/library3372.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/convolution_large/solutions/library3372.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <iostream>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"
#include "library3372/math/convolution_arbitrary_mod.hpp"
#include "library3372/math/static_modint.hpp"

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n, m;
  std::cin >> n >> m;
  using Mint = library3372::StaticModInt<998244353>;
  std::vector<Mint> a(n), b(m);
  for (int i = 0; i < n; ++i) {
    int x;
    std::cin >> x;
    a[i] = Mint::raw(x);
  }
  for (int i = 0; i < m; ++i) {
    int x;
    std::cin >> x;
    b[i] = Mint::raw(x);
  }

  library3372::internal::Timer timer;
  std::vector<Mint> c = library3372::convolution_arbitrary_mod(a, b);
  double run_sec = timer.elapsed();

  unsigned long long ans_hash = 0;
  for (const Mint& x : c) {
    ans_hash = library3372::internal::hash_combine(ans_hash,
                                                   (unsigned long long)x.val());
  }

  std::cout << "hash " << ans_hash << '\n';
  std::cout << "time run " << run_sec << '\n';
  return 0;
}
