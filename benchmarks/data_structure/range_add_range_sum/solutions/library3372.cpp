// Repository: Library3372
// File: benchmarks/data_structure/range_add_range_sum/solutions/library3372.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/data_structure/range_add_range_sum/solutions/library3372.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <iostream>
#include <vector>

#include "library3372/data_structure/range_add_range_sum.hpp"
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
  std::vector<long long> init(n);
  for (int i = 0; i < n; ++i) std::cin >> init[i];

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
  library3372::RangeAddRangeSum<long long> fw(init);
  double build_sec = timer.elapsed();

  timer.reset();
  unsigned long long ans_hash = 0;
  for (const auto& qry : queries) {
    if (qry.type == 0) {
      fw.add(qry.a, qry.b, qry.x);
    } else if (qry.type == 1) {
      long long res = fw.sum(qry.a, qry.b);
      ans_hash = library3372::internal::hash_combine(ans_hash,
                                                     (unsigned long long)res);
    } else if (qry.type == 2) {
      int res = fw.max_right(qry.a, qry.x);
      ans_hash = library3372::internal::hash_combine(ans_hash,
                                                     (unsigned long long)res);
    } else {
      int res = fw.min_left(qry.a, qry.x);
      ans_hash = library3372::internal::hash_combine(ans_hash,
                                                     (unsigned long long)res);
    }
  }
  double queries_sec = timer.elapsed();

  std::cout << "hash " << ans_hash << "\n";
  std::cout << "time build " << build_sec << "\n";
  std::cout << "time queries " << queries_sec << "\n";
  return 0;
}
