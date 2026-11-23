// Repository: Library3372
// File: benchmarks/math/convolution_mod_1000000007/solutions/naive.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/convolution_mod_1000000007/solutions/naive.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <iostream>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n, m;
  std::cin >> n >> m;
  std::vector<unsigned int> a(n), b(m);
  for (int i = 0; i < n; ++i) std::cin >> a[i];
  for (int i = 0; i < m; ++i) std::cin >> b[i];

  if ((long long)n * m > 100000000LL) {
    std::cout << "skip N * M > 10^8\n";
    return 0;
  }

  library3372::internal::Timer timer;
  std::vector<unsigned long long> c(n + m - 1);
  constexpr unsigned long long mod = 1000000007ULL;
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      c[i + j] = (c[i + j] + (unsigned long long)a[i] * b[j]) % mod;
    }
  }
  double run_sec = timer.elapsed();

  unsigned long long ans_hash = 0;
  for (int i = 0; i < (int)c.size(); ++i) {
    ans_hash = library3372::internal::hash_combine(ans_hash, c[i]);
  }

  std::cout << "hash " << ans_hash << '\n';
  std::cout << "time run " << run_sec << '\n';
  return 0;
}
