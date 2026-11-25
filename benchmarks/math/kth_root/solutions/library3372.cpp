// Repository: Library3372
// File: benchmarks/math/kth_root/solutions/library3372.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/kth_root/solutions/library3372.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <iostream>
#include <utility>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"
#include "library3372/math/kth_root.hpp"

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int q;
  std::cin >> q;
  std::vector<std::pair<long long, long long>> qs(q);
  for (int i = 0; i < q; ++i) std::cin >> qs[i].first >> qs[i].second;

  library3372::internal::Timer timer;
  std::vector<long long> res(q);
  for (int i = 0; i < q; ++i) {
    res[i] = library3372::kth_root(qs[i].first, qs[i].second);
  }
  double run_sec = timer.elapsed();

  unsigned long long ans_hash = 0;
  for (int i = 0; i < q; ++i) {
    ans_hash = library3372::internal::hash_combine(ans_hash,
                                                   (unsigned long long)res[i]);
  }

  std::cout << "hash " << ans_hash << '\n';
  std::cout << "time run " << run_sec << '\n';
  return 0;
}
