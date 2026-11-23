// Repository: Library3372
// File: benchmarks/math/enumerate_quotients/solutions/simple_64bit.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/enumerate_quotients/solutions/simple_64bit.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <iostream>
#include <tuple>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"

namespace {

std::vector<std::tuple<long long, long long, long long>> simple_enumerate(
    long long n) {
  if (n == 0) return {};
  std::vector<std::tuple<long long, long long, long long>> res;
  long long r = n + 1;
  for (long long q = 1;; ++q) {
    long long l = n / (q + 1) + 1;
    res.emplace_back(q, l, r);
    r = l;
    if (q >= l - 1) break;
  }
  for (long long x = r - 1; x >= 1; --x) {
    res.emplace_back(n / x, x, x + 1);
  }
  return res;
}

}  // namespace

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int q_cnt;
  std::cin >> q_cnt;
  std::vector<long long> a(q_cnt);
  for (int i = 0; i < q_cnt; ++i) std::cin >> a[i];

  library3372::internal::Timer timer;
  std::vector<std::vector<std::tuple<long long, long long, long long>>> res(
      q_cnt);
  for (int i = 0; i < q_cnt; ++i) res[i] = simple_enumerate(a[i]);
  double run_sec = timer.elapsed();

  unsigned long long ans_hash = 0;
  for (int i = 0; i < q_cnt; ++i) {
    ans_hash = library3372::internal::hash_combine(ans_hash, res[i].size());
    for (const auto& [q, l, r] : res[i]) {
      ans_hash =
          library3372::internal::hash_combine(ans_hash, (unsigned long long)q);
      ans_hash =
          library3372::internal::hash_combine(ans_hash, (unsigned long long)l);
      ans_hash =
          library3372::internal::hash_combine(ans_hash, (unsigned long long)r);
    }
  }

  std::cout << "hash " << ans_hash << '\n';
  std::cout << "time run " << run_sec << '\n';
  return 0;
}
