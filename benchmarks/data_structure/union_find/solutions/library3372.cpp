// Repository: Library3372
// File: benchmarks/data_structure/union_find/solutions/library3372.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/data_structure/union_find/solutions/library3372.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <iostream>
#include <vector>

#include "library3372/data_structure/union_find.hpp"
#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"

namespace {

struct Query {
  int type;
  int u;
  int v;
};

}  // namespace

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n, q;
  std::cin >> n >> q;
  std::vector<Query> queries(q);
  for (int i = 0; i < q; ++i) {
    int t;
    std::cin >> t;
    queries[i].type = t;
    if (t == 0 || t == 1) {
      std::cin >> queries[i].u >> queries[i].v;
    } else if (t == 2 || t == 3) {
      std::cin >> queries[i].u;
    }
  }

  library3372::internal::Timer timer;
  library3372::UnionFind uf(n);
  double init_sec = timer.elapsed();

  timer.reset();
  unsigned long long ans_hash = 0;
  for (const auto& qry : queries) {
    if (qry.type == 0) {
      auto res = uf.unite(qry.u, qry.v);
      if (res.has_value()) {
        ans_hash = library3372::internal::hash_combine(
            ans_hash, (unsigned long long)res->first);
        ans_hash = library3372::internal::hash_combine(
            ans_hash, (unsigned long long)res->second);
      } else {
        ans_hash = library3372::internal::hash_combine(ans_hash, 0ULL);
      }
    } else if (qry.type == 1) {
      ans_hash = library3372::internal::hash_combine(
          ans_hash, (unsigned long long)uf.same(qry.u, qry.v));
    } else if (qry.type == 2) {
      ans_hash = library3372::internal::hash_combine(
          ans_hash, (unsigned long long)uf.size(qry.u));
    } else if (qry.type == 3) {
      ans_hash = library3372::internal::hash_combine(
          ans_hash, (unsigned long long)uf.find(qry.u));
    } else {
      ans_hash = library3372::internal::hash_combine(
          ans_hash, (unsigned long long)uf.count_groups());
    }
  }
  double queries_sec = timer.elapsed();

  timer.reset();
  auto gs = uf.groups();
  double groups_sec = timer.elapsed();

  unsigned long long groups_hash = 0;
  for (const auto& g : gs) {
    unsigned long long gh = (unsigned long long)g.size();
    for (int v : g) {
      gh = library3372::internal::hash_combine(gh, (unsigned long long)v);
    }
    groups_hash += gh;
  }
  ans_hash = library3372::internal::hash_combine(ans_hash, groups_hash);

  std::cout << "hash " << ans_hash << "\n";
  std::cout << "time init " << init_sec << "\n";
  std::cout << "time queries " << queries_sec << "\n";
  std::cout << "time groups " << groups_sec << "\n";
  return 0;
}
