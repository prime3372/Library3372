// Repository: Library3372
// File: benchmarks/data_structure/disjoint_sparse_table/solutions/naive.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/data_structure/disjoint_sparse_table/solutions/naive.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <iostream>
#include <utility>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"

namespace {

constexpr unsigned long long kMod = 998244353ULL;

struct Affine {
  unsigned int a;
  unsigned int b;
};

Affine affine_op(const Affine& f, const Affine& g) {
  return {
      (unsigned int)((unsigned long long)g.a * f.a % kMod),
      (unsigned int)(((unsigned long long)g.a * f.b + g.b) % kMod),
  };
}

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

  std::vector<Affine> a(n);
  for (int i = 0; i < n; ++i) std::cin >> a[i].a >> a[i].b;

  std::vector<std::pair<int, int>> queries(q);
  for (int i = 0; i < q; ++i) {
    std::cin >> queries[i].first >> queries[i].second;
  }

  library3372::internal::Timer timer;
  unsigned long long ans_hash = 0;
  for (const auto& [l, r] : queries) {
    Affine res = {1U, 0U};
    for (int i = l; i < r; ++i) res = affine_op(res, a[i]);
    ans_hash = library3372::internal::hash_combine(ans_hash, res.a);
    ans_hash = library3372::internal::hash_combine(ans_hash, res.b);
  }
  double queries_sec = timer.elapsed();

  std::cout << "hash " << ans_hash << "\n";
  std::cout << "time queries " << queries_sec << "\n";
  return 0;
}
