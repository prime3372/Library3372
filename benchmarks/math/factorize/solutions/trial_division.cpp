// Repository: Library3372
// File: benchmarks/math/factorize/solutions/trial_division.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/factorize/solutions/trial_division.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <algorithm>
#include <iostream>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"

namespace {

std::vector<long long> trial_factorize(long long n) {
  std::vector<long long> res;
  for (long long d = 2; d <= n / d; ++d) {
    while (n % d == 0) {
      res.push_back(d);
      n /= d;
    }
  }
  if (n > 1) res.push_back(n);
  return res;
}

}  // namespace

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int q;
  std::cin >> q;
  std::vector<long long> a(q);
  long long max_a = 0;
  for (int i = 0; i < q; ++i) {
    std::cin >> a[i];
    max_a = std::max(max_a, a[i]);
  }

  if (max_a > 10000000000LL) {
    std::cout << "skip max(A) > 10^10\n";
    return 0;
  }

  library3372::internal::Timer timer;
  std::vector<std::vector<long long>> res(q);
  for (int i = 0; i < q; ++i) res[i] = trial_factorize(a[i]);
  double run_sec = timer.elapsed();

  unsigned long long ans_hash = 0;
  for (int i = 0; i < q; ++i) {
    ans_hash = library3372::internal::hash_combine(ans_hash, res[i].size());
    for (long long p : res[i]) {
      ans_hash =
          library3372::internal::hash_combine(ans_hash, (unsigned long long)p);
    }
  }

  std::cout << "hash " << ans_hash << '\n';
  std::cout << "time run " << run_sec << '\n';
  return 0;
}
