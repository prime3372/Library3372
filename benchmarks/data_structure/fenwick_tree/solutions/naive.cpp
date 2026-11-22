// Repository: Library3372
// File: benchmarks/data_structure/fenwick_tree/solutions/naive.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/data_structure/fenwick_tree/solutions/naive.cpp
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

  if ((long long)n * q > 50000000LL) {
    std::cout << "skip N * Q > 50000000\n";
    return 0;
  }

  library3372::internal::Timer timer;
  std::vector<long long> a = init;
  double build_sec = timer.elapsed();

  timer.reset();
  unsigned long long ans_hash = 0;
  for (const auto& qry : queries) {
    if (qry.type == 0) {
      a[qry.a] += qry.b;
    } else if (qry.type == 1) {
      a[qry.a] = qry.b;
    } else if (qry.type == 2) {
      ans_hash = library3372::internal::hash_combine(
          ans_hash, (unsigned long long)a[qry.a]);
    } else if (qry.type == 3) {
      int r = (int)qry.b;
      long long res = 0;
      for (int i = qry.a; i < r; ++i) res += a[i];
      ans_hash = library3372::internal::hash_combine(ans_hash,
                                                     (unsigned long long)res);
    } else if (qry.type == 4) {
      long long s = 0;
      int r = qry.a;
      while (r < n && s + a[r] <= qry.b) {
        s += a[r];
        ++r;
      }
      ans_hash =
          library3372::internal::hash_combine(ans_hash, (unsigned long long)r);
    } else {
      long long s = 0;
      int l = qry.a;
      while (l > 0 && s + a[l - 1] <= qry.b) {
        s += a[l - 1];
        --l;
      }
      ans_hash =
          library3372::internal::hash_combine(ans_hash, (unsigned long long)l);
    }
  }
  double queries_sec = timer.elapsed();

  std::cout << "hash " << ans_hash << "\n";
  std::cout << "time build " << build_sec << "\n";
  std::cout << "time queries " << queries_sec << "\n";
  return 0;
}
