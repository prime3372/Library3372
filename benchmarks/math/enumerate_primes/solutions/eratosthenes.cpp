// Repository: Library3372
// File: benchmarks/math/enumerate_primes/solutions/eratosthenes.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/enumerate_primes/solutions/eratosthenes.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <iostream>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n;
  std::cin >> n;

  library3372::internal::Timer timer;
  std::vector<unsigned char> is_p(n + 1, 1);
  if (n >= 0) is_p[0] = 0;
  if (n >= 1) is_p[1] = 0;
  for (int i = 2; (long long)i * i <= n; ++i) {
    if (!is_p[i]) continue;
    for (int j = i * i; j <= n; j += i) is_p[j] = 0;
  }
  std::vector<int> primes;
  for (int i = 2; i <= n; ++i) {
    if (is_p[i]) primes.push_back(i);
  }
  double run_sec = timer.elapsed();

  unsigned long long ans_hash = 0;
  for (int p : primes) {
    ans_hash =
        library3372::internal::hash_combine(ans_hash, (unsigned long long)p);
  }

  std::cout << "hash " << ans_hash << '\n';
  std::cout << "time run " << run_sec << '\n';
  return 0;
}
