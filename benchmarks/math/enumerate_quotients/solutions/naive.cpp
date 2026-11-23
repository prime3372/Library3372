// Repository: Library3372
// File: benchmarks/math/enumerate_quotients/solutions/naive.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/enumerate_quotients/solutions/naive.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <iostream>
#include <tuple>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"

namespace {

std::vector<std::tuple<long long, long long, long long>> naive_enumerate(
    long long n) {
  std::vector<std::tuple<long long, long long, long long>> res;
  for (long long r = n + 1; r > 1;) {
    long long x = r - 1;
    long long q = n / x;
    long long l = x;
    while (l > 1 && n / (l - 1) == q) --l;
    res.emplace_back(q, l, r);
    r = l;
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
  long long sum_a = 0;
  for (int i = 0; i < q_cnt; ++i) {
    std::cin >> a[i];
    if (a[i] > 100000000LL - sum_a) {
      sum_a = 100000001LL;
    } else {
      sum_a += a[i];
    }
  }

  if (sum_a > 100000000LL) {
    std::cout << "skip sum(A) > 10^8\n";
    return 0;
  }

  library3372::internal::Timer timer;
  std::vector<std::vector<std::tuple<long long, long long, long long>>> res(
      q_cnt);
  for (int i = 0; i < q_cnt; ++i) res[i] = naive_enumerate(a[i]);
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
