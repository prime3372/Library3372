// Repository: Library3372
// File: benchmarks/data_structure/fenwick_tree/solutions/segment_tree.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/data_structure/fenwick_tree/solutions/segment_tree.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include "library3372/data_structure/segment_tree.hpp"

#include <iostream>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"

namespace {

struct Query {
  int type;
  int a;
  long long b;
};

long long sum_op(long long a, long long b) { return a + b; }
long long sum_e() { return 0LL; }

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
  library3372::SegmentTree<long long, sum_op, sum_e> seg(init);
  double build_sec = timer.elapsed();

  timer.reset();
  unsigned long long ans_hash = 0;
  for (const auto& qry : queries) {
    if (qry.type == 0) {
      seg.set(qry.a, seg.get(qry.a) + qry.b);
    } else if (qry.type == 1) {
      seg.set(qry.a, qry.b);
    } else if (qry.type == 2) {
      long long res = seg.get(qry.a);
      ans_hash = library3372::internal::hash_combine(ans_hash,
                                                     (unsigned long long)res);
    } else if (qry.type == 3) {
      long long res = seg.prod(qry.a, (int)qry.b);
      ans_hash = library3372::internal::hash_combine(ans_hash,
                                                     (unsigned long long)res);
    } else if (qry.type == 4) {
      long long limit = qry.b;
      int res =
          seg.max_right(qry.a, [limit](long long s) { return s <= limit; });
      ans_hash = library3372::internal::hash_combine(ans_hash,
                                                     (unsigned long long)res);
    } else {
      long long limit = qry.b;
      int res =
          seg.min_left(qry.a, [limit](long long s) { return s <= limit; });
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
