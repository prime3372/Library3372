// Repository: Library3372
// File: benchmarks/data_structure/dual_fenwick_tree/solutions/lazy_segment_tree.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/data_structure/dual_fenwick_tree/solutions/lazy_segment_tree.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include "library3372/data_structure/lazy_segment_tree.hpp"

#include <algorithm>
#include <iostream>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"

namespace {

long long op(long long a, long long b) { return std::max(a, b); }
long long e() { return -(1LL << 62); }
long long mapping(long long f, long long x) { return f + x; }
long long composition(long long f, long long g) { return f + g; }
long long id() { return 0; }

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
    if (queries[i].type == 0) {
      std::cin >> queries[i].b >> queries[i].x;
    } else if (queries[i].type == 1) {
      std::cin >> queries[i].x;
    }
  }

  library3372::internal::Timer timer;
  library3372::LazySegmentTree<long long, op, e, long long, mapping,
                               composition, id>
      seg(init);
  double build_sec = timer.elapsed();

  timer.reset();
  unsigned long long ans_hash = 0;
  for (const auto& qry : queries) {
    if (qry.type == 0) {
      seg.apply(qry.a, qry.b, qry.x);
    } else if (qry.type == 1) {
      seg.set(qry.a, qry.x);
    } else {
      long long res = seg.get(qry.a);
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
