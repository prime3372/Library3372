// Repository: Library3372
// File: benchmarks/data_structure/range_add_range_sum/solutions/lazy_segment_tree.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/data_structure/range_add_range_sum/solutions/lazy_segment_tree.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include "library3372/data_structure/lazy_segment_tree.hpp"

#include <iostream>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"

namespace {

struct S {
  long long sum;
  int len;
};

S op(const S& a, const S& b) { return {a.sum + b.sum, a.len + b.len}; }
S e() { return {0, 0}; }
S mapping(long long f, const S& x) { return {x.sum + f * x.len, x.len}; }
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
  std::vector<S> init(n);
  for (int i = 0; i < n; ++i) {
    std::cin >> init[i].sum;
    init[i].len = 1;
  }

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
  library3372::LazySegmentTree<S, op, e, long long, mapping, composition, id>
      seg(init);
  double build_sec = timer.elapsed();

  timer.reset();
  unsigned long long ans_hash = 0;
  for (const auto& qry : queries) {
    if (qry.type == 0) {
      seg.apply(qry.a, qry.b, qry.x);
    } else if (qry.type == 1) {
      long long res = seg.prod(qry.a, qry.b).sum;
      ans_hash = library3372::internal::hash_combine(ans_hash,
                                                     (unsigned long long)res);
    } else if (qry.type == 2) {
      int res = seg.max_right(
          qry.a, [lim = qry.x](const S& s) { return s.sum <= lim; });
      ans_hash = library3372::internal::hash_combine(ans_hash,
                                                     (unsigned long long)res);
    } else {
      int res = seg.min_left(
          qry.a, [lim = qry.x](const S& s) { return s.sum <= lim; });
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
