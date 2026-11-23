// Repository: Library3372
// File: benchmarks/math/convolution_mod_2_64/solutions/scalar.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/convolution_mod_2_64/solutions/scalar.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#pragma GCC push_options
#pragma GCC target("no-avx2")

#include <iostream>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"
#include "library3372/math/convolution_mod_2_64.hpp"

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n, m;
  std::cin >> n >> m;
  std::vector<unsigned long long> a(n), b(m);
  for (int i = 0; i < n; ++i) std::cin >> a[i];
  for (int i = 0; i < m; ++i) std::cin >> b[i];

  library3372::internal::Timer timer;
  std::vector<unsigned long long> c = library3372::convolution_mod_2_64(a, b);
  double run_sec = timer.elapsed();

  unsigned long long ans_hash = 0;
  for (unsigned long long x : c) {
    ans_hash = library3372::internal::hash_combine(ans_hash, x);
  }

  std::cout << "hash " << ans_hash << '\n';
  std::cout << "time run " << run_sec << '\n';
  return 0;
}

#pragma GCC pop_options
