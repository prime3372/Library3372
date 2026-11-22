// Repository: Library3372
// File: benchmarks/data_structure/fenwick_tree/solutions/library3372.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/data_structure/fenwick_tree/solutions/library3372.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <iostream>
#include <vector>

#include "library3372/data_structure/fenwick_tree.hpp"
#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"

namespace {

struct Query {
  int type;
  int a;
  long long b;
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
    if (queries[i].type != 2) std::cin >> queries[i].b;
  }

  library3372::internal::Timer timer;
  library3372::FenwickTree<long long> ft(init);
  double build_sec = timer.elapsed();

  timer.reset();
  unsigned long long ans_hash = 0;
  for (const auto& qry : queries) {
    if (qry.type == 0) {
      ft.add(qry.a, qry.b);
    } else if (qry.type == 1) {
      ft.set(qry.a, qry.b);
    } else if (qry.type == 2) {
      long long res = ft.get(qry.a);
      ans_hash = library3372::internal::hash_combine(ans_hash,
                                                     (unsigned long long)res);
    } else if (qry.type == 3) {
      long long res = ft.sum(qry.a, (int)qry.b);
      ans_hash = library3372::internal::hash_combine(ans_hash,
                                                     (unsigned long long)res);
    } else if (qry.type == 4) {
      long long lim = qry.b;
      int res = ft.max_right(qry.a, [&](long long s) { return s <= lim; });
      ans_hash = library3372::internal::hash_combine(ans_hash,
                                                     (unsigned long long)res);
    } else {
      long long lim = qry.b;
      int res = ft.min_left(qry.a, [&](long long s) { return s <= lim; });
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
