// Repository: Library3372
// File: benchmarks/data_structure/range_add_range_sum/solutions/naive.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/data_structure/range_add_range_sum/solutions/naive.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <iostream>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"

namespace {

struct Query {
  int type;
  int a;
  int b;
  long long x;
};

}  // namespace

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n, q;
  std::cin >> n >> q;
  if ((long long)n * q > 100000000LL) {
    std::cout << "skip N * Q > 10^8\n";
    return 0;
  }

  std::vector<long long> a(n);
  for (int i = 0; i < n; ++i) std::cin >> a[i];

  std::vector<Query> queries(q);
  for (int i = 0; i < q; ++i) {
    std::cin >> queries[i].type >> queries[i].a;
    if (queries[i].type <= 1) {
      std::cin >> queries[i].b;
      if (queries[i].type == 0) std::cin >> queries[i].x;
    } else {
      std::cin >> queries[i].x;
    }
  }

  library3372::internal::Timer timer;
  unsigned long long ans_hash = 0;
  for (const auto& qry : queries) {
    if (qry.type == 0) {
      for (int i = qry.a; i < qry.b; ++i) a[i] += qry.x;
    } else if (qry.type == 1) {
      long long res = 0;
      for (int i = qry.a; i < qry.b; ++i) res += a[i];
      ans_hash = library3372::internal::hash_combine(ans_hash,
                                                     (unsigned long long)res);
    } else if (qry.type == 2) {
      long long s = 0;
      int r = qry.a;
      while (r < n && s + a[r] <= qry.x) {
        s += a[r];
        ++r;
      }
      ans_hash =
          library3372::internal::hash_combine(ans_hash, (unsigned long long)r);
    } else {
      long long s = 0;
      int l = qry.a;
      while (l > 0 && s + a[l - 1] <= qry.x) {
        s += a[l - 1];
        --l;
      }
      ans_hash =
          library3372::internal::hash_combine(ans_hash, (unsigned long long)l);
    }
  }
  double queries_sec = timer.elapsed();

  std::cout << "hash " << ans_hash << "\n";
  std::cout << "time queries " << queries_sec << "\n";
  return 0;
}
