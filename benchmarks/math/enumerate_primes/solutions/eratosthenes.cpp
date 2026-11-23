// Repository: Library3372
// File: benchmarks/math/enumerate_primes/solutions/eratosthenes.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/enumerate_primes/solutions/eratosthenes.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cmath>
#include <iostream>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int t;
  long long l, r;
  std::cin >> t >> l >> r;

  if (t == 2 && r > 100000000000000LL) {
    std::cout << "skip R > 1e14\n";
    return 0;
  }

  unsigned long long ans_hash = 0;
  double run_sec = 0.0;

  if (t == 1) {
    int n = (int)r;
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
    run_sec = timer.elapsed();
    for (int p : primes) {
      ans_hash =
          library3372::internal::hash_combine(ans_hash, (unsigned long long)p);
    }
  } else {
    library3372::internal::Timer timer;
    long long limit = (long long)std::sqrt((double)(r - 1));
    while ((limit + 1) * (limit + 1) <= r - 1) ++limit;
    while (limit * limit > r - 1) --limit;
    std::vector<unsigned char> small_p(limit + 1, 1);
    if (limit >= 0) small_p[0] = 0;
    if (limit >= 1) small_p[1] = 0;
    for (long long i = 2; i * i <= limit; ++i) {
      if (!small_p[i]) continue;
      for (long long j = i * i; j <= limit; j += i) small_p[j] = 0;
    }
    std::vector<unsigned char> seg(r - l, 1);
    if (l == 0 && r > 0) seg[0] = 0;
    if (l <= 1 && 1 < r) seg[1 - l] = 0;
    for (long long p = 2; p <= limit; ++p) {
      if (!small_p[p]) continue;
      long long start = ((l + p - 1) / p) * p;
      if (start < p * p) start = p * p;
      for (long long j = start; j < r; j += p) seg[j - l] = 0;
    }
    std::vector<long long> primes;
    for (long long i = 0; i < r - l; ++i) {
      if (seg[i]) primes.push_back(l + i);
    }
    run_sec = timer.elapsed();
    for (long long p : primes) {
      ans_hash =
          library3372::internal::hash_combine(ans_hash, (unsigned long long)p);
    }
  }

  std::cout << "hash " << ans_hash << '\n';
  std::cout << "time run " << run_sec << '\n';
  return 0;
}
