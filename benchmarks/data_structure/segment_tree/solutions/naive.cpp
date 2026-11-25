// Repository: Library3372
// File: benchmarks/data_structure/segment_tree/solutions/naive.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/data_structure/segment_tree/solutions/naive.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <algorithm>
#include <iostream>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"

namespace {

struct Query {
  int type;
  int a;
  int b;
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

  std::vector<int> a(n);
  for (int i = 0; i < n; ++i) std::cin >> a[i];

  std::vector<Query> queries(q);
  for (int i = 0; i < q; ++i) {
    std::cin >> queries[i].type >> queries[i].a;
    if (queries[i].type != 1) std::cin >> queries[i].b;
  }

  library3372::internal::Timer timer;
  unsigned long long ans_hash = 0;
  for (const auto& qry : queries) {
    if (qry.type == 0) {
      a[qry.a] = qry.b;
    } else if (qry.type == 1) {
      ans_hash = library3372::internal::hash_combine(
          ans_hash, (unsigned long long)a[qry.a]);
    } else if (qry.type == 2) {
      int res = 0;
      for (int i = qry.a; i < qry.b; ++i) res = std::max(res, a[i]);
      ans_hash = library3372::internal::hash_combine(ans_hash,
                                                     (unsigned long long)res);
    } else if (qry.type == 3) {
      int r = qry.a;
      while (r < n && a[r] <= qry.b) ++r;
      ans_hash =
          library3372::internal::hash_combine(ans_hash, (unsigned long long)r);
    } else {
      int l = qry.a;
      while (l > 0 && a[l - 1] <= qry.b) --l;
      ans_hash =
          library3372::internal::hash_combine(ans_hash, (unsigned long long)l);
    }
  }
  double queries_sec = timer.elapsed();

  std::cout << "hash " << ans_hash << "\n";
  std::cout << "time queries " << queries_sec << "\n";
  return 0;
}
