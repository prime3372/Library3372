// Repository: Library3372
// File: benchmarks/math/sqrt_mod/solutions/library3372.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/sqrt_mod/solutions/library3372.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <iostream>
#include <optional>
#include <utility>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"
#include "library3372/math/sqrt_mod.hpp"

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int q;
  std::cin >> q;
  std::vector<std::pair<long long, long long>> qs(q);
  for (int i = 0; i < q; ++i) std::cin >> qs[i].first >> qs[i].second;

  library3372::internal::Timer timer;
  std::vector<std::optional<long long>> res(q);
  for (int i = 0; i < q; ++i) {
    res[i] = library3372::sqrt_mod(qs[i].first, qs[i].second);
  }
  double run_sec = timer.elapsed();

  unsigned long long ans_hash = 0;
  for (int i = 0; i < q; ++i) {
    unsigned long long v =
        res[i].has_value() ? (unsigned long long)(*res[i]) : ~0ULL;
    ans_hash = library3372::internal::hash_combine(ans_hash, v);
  }

  std::cout << "hash " << ans_hash << '\n';
  std::cout << "time run " << run_sec << '\n';
  return 0;
}
