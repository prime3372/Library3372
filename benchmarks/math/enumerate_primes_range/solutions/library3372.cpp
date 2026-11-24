// Repository: Library3372
// File: benchmarks/math/enumerate_primes_range/solutions/library3372.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/enumerate_primes_range/solutions/library3372.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <iostream>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"
#include "library3372/math/enumerate_primes.hpp"

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  long long l, r;
  std::cin >> l >> r;

  library3372::internal::Timer timer;
  std::vector<long long> primes = library3372::enumerate_primes(l, r);
  double run_sec = timer.elapsed();

  unsigned long long ans_hash = 0;
  for (long long p : primes) {
    ans_hash =
        library3372::internal::hash_combine(ans_hash, (unsigned long long)p);
  }

  std::cout << "hash " << ans_hash << '\n';
  std::cout << "time run " << run_sec << '\n';
  return 0;
}
