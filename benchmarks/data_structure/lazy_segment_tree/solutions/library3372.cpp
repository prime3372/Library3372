// Repository: Library3372
// File: benchmarks/data_structure/lazy_segment_tree/solutions/library3372.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/data_structure/lazy_segment_tree/solutions/library3372.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <iostream>
#include <vector>

#include "library3372/data_structure/lazy_segment_tree.hpp"
#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"

namespace {

constexpr unsigned long long kMod = 998244353ULL;

struct S {
  unsigned int sum;
  unsigned int len;
};

struct F {
  unsigned int b;
  unsigned int c;
};

S op(const S& l, const S& r) {
  unsigned int s = l.sum + r.sum;
  if (s >= kMod) s -= (unsigned int)kMod;
  return {s, l.len + r.len};
}

S e() { return {0U, 0U}; }

S mapping(const F& f, const S& x) {
  return {(unsigned int)(((unsigned long long)f.b * x.sum +
                          (unsigned long long)f.c * x.len) %
                         kMod),
          x.len};
}

F composition(const F& f, const F& g) {
  return {(unsigned int)((unsigned long long)f.b * g.b % kMod),
          (unsigned int)(((unsigned long long)f.b * g.c + f.c) % kMod)};
}

F id() { return {1U, 0U}; }

struct Query {
  int type;
  int a;
  int b;
  unsigned int c;
  unsigned int d;
};

}  // namespace

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n, q;
  std::cin >> n >> q;
  std::vector<S> init(n);
  for (int i = 0; i < n; ++i) {
    unsigned int v;
    std::cin >> v;
    init[i] = {v, 1U};
  }

  std::vector<Query> queries(q);
  for (int i = 0; i < q; ++i) {
    std::cin >> queries[i].type >> queries[i].a;
    if (queries[i].type == 0) {
      std::cin >> queries[i].b >> queries[i].c >> queries[i].d;
    } else if (queries[i].type == 1) {
      std::cin >> queries[i].c;
    } else if (queries[i].type == 3) {
      std::cin >> queries[i].b;
    }
  }

  library3372::internal::Timer timer;
  library3372::LazySegmentTree<S, op, e, F, mapping, composition, id> seg(init);
  double build_sec = timer.elapsed();

  timer.reset();
  unsigned long long ans_hash = 0;
  for (const auto& qry : queries) {
    if (qry.type == 0) {
      seg.apply(qry.a, qry.b, {qry.c, qry.d});
    } else if (qry.type == 1) {
      seg.set(qry.a, {qry.c, 1U});
    } else if (qry.type == 2) {
      unsigned int res = seg.get(qry.a).sum;
      ans_hash = library3372::internal::hash_combine(ans_hash, res);
    } else {
      unsigned int res = seg.prod(qry.a, qry.b).sum;
      ans_hash = library3372::internal::hash_combine(ans_hash, res);
    }
  }
  double queries_sec = timer.elapsed();

  std::cout << "hash " << ans_hash << "\n";
  std::cout << "time build " << build_sec << "\n";
  std::cout << "time queries " << queries_sec << "\n";
  return 0;
}
