// Repository: Library3372
// File: benchmarks/math/floor_sum/solutions/naive.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/floor_sum/solutions/naive.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <iostream>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"

namespace {

struct Query {
  long long n, m, a, b;
};

unsigned long long naive_floor_sum(long long n, long long m, long long a,
                                   long long b) {
  unsigned long long sum = 0;
  for (long long i = 0; i < n; ++i) {
    __int128 num = (__int128)a * i + b;
    __int128 q = num / m;
    if (num % m < 0) --q;
    sum += (unsigned long long)q;
  }
  return sum;
}

}  // namespace

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int q;
  std::cin >> q;
  std::vector<Query> queries(q);
  long long sum_n = 0;
  for (int i = 0; i < q; ++i) {
    std::cin >> queries[i].n >> queries[i].m >> queries[i].a >> queries[i].b;
    if (queries[i].n > 100000000LL - sum_n) {
      sum_n = 100000001LL;
    } else {
      sum_n += queries[i].n;
    }
  }

  if (sum_n > 100000000LL) {
    std::cout << "skip sum(N) > 10^8\n";
    return 0;
  }

  library3372::internal::Timer timer;
  std::vector<unsigned long long> res(q);
  for (int i = 0; i < q; ++i) {
    res[i] =
        naive_floor_sum(queries[i].n, queries[i].m, queries[i].a, queries[i].b);
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
