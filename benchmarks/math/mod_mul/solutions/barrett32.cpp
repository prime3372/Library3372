// Repository: Library3372
// File: benchmarks/math/mod_mul/solutions/barrett32.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/mod_mul/solutions/barrett32.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <iostream>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"
#include "library3372/internal/math/barrett.hpp"

struct Query {
  unsigned long long m;
  unsigned long long x;
  unsigned long long y;
  int k;
};

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int q;
  std::cin >> q;
  std::vector<Query> queries(q);
  bool overflow = false;
  for (int i = 0; i < q; ++i) {
    std::cin >> queries[i].m >> queries[i].x >> queries[i].y >> queries[i].k;
    if (queries[i].m > 0xffffffffULL) overflow = true;
  }
  if (overflow) {
    std::cout << "skip M >= 2^32\n";
    return 0;
  }

  library3372::internal::Timer timer;
  std::vector<unsigned long long> ans(2 * q);
  for (int i = 0; i < q; ++i) {
    library3372::internal::Barrett32 bt((unsigned int)queries[i].m);
    unsigned int a = (unsigned int)queries[i].x;
    unsigned int b = (unsigned int)queries[i].y;
    int k = queries[i].k;
    for (int j = 0; j < k; ++j) {
      a = bt.mul(a, b);
      b = bt.mul(b, a);
    }
    ans[2 * i] = a;
    ans[2 * i + 1] = b;
  }
  double run_sec = timer.elapsed();

  unsigned long long ans_hash = 0;
  for (unsigned long long v : ans) {
    ans_hash = library3372::internal::hash_combine(ans_hash, v);
  }

  std::cout << "hash " << ans_hash << '\n';
  std::cout << "time run " << run_sec << '\n';
  return 0;
}
