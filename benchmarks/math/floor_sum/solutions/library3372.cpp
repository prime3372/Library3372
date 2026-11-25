// Repository: Library3372
// File: benchmarks/math/floor_sum/solutions/library3372.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/floor_sum/solutions/library3372.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <iostream>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"
#include "library3372/math/floor_sum.hpp"

namespace {

struct Query {
  long long n, m, a, b;
};

}  // namespace

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int q;
  std::cin >> q;
  std::vector<Query> queries(q);
  for (int i = 0; i < q; ++i) {
    std::cin >> queries[i].n >> queries[i].m >> queries[i].a >> queries[i].b;
  }

  library3372::internal::Timer timer;
  std::vector<unsigned long long> res(q);
  for (int i = 0; i < q; ++i) {
    res[i] = (unsigned long long)library3372::floor_sum(
        queries[i].n, queries[i].m, queries[i].a, queries[i].b);
  }
  double run_sec = timer.elapsed();

  unsigned long long ans_hash = 0;
  for (int i = 0; i < q; ++i) {
    ans_hash = library3372::internal::hash_combine(ans_hash, res[i]);
  }

  std::cout << "hash " << ans_hash << '\n';
  std::cout << "time run " << run_sec << '\n';
  return 0;
}
