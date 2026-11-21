// Repository: Library3372
// File: benchmarks/data_structure/union_find/solutions/naive.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/data_structure/union_find/solutions/naive.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <iostream>
#include <optional>
#include <utility>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"

namespace {

struct Query {
  int type;
  int u;
  int v;
};

class NaiveUnionFind {
 public:
  explicit NaiveUnionFind(int n)
      : root_(n), comp_size_(n, 1), count_groups_(n) {
    for (int i = 0; i < n; ++i) root_[i] = i;
  }

  int find(int x) const { return root_[x]; }

  std::optional<std::pair<int, int>> unite(int x, int y) {
    x = root_[x];
    y = root_[y];
    if (x == y) return std::nullopt;
    if (comp_size_[x] < comp_size_[y]) std::swap(x, y);
    for (int& r : root_) {
      if (r == y) r = x;
    }
    comp_size_[x] += comp_size_[y];
    comp_size_[y] = 0;
    --count_groups_;
    return std::pair<int, int>{x, y};
  }

  bool same(int x, int y) const { return root_[x] == root_[y]; }

  int size(int x) const { return comp_size_[root_[x]]; }

  int count_groups() const { return count_groups_; }

  std::vector<std::vector<int>> groups() const {
    int n = (int)root_.size();
    std::vector<int> group_id(n, -1);
    std::vector<std::vector<int>> result(count_groups_);
    int next_id = 0;
    for (int i = 0; i < n; ++i) {
      int r = root_[i];
      if (group_id[r] == -1) group_id[r] = next_id++;
      result[group_id[r]].push_back(i);
    }
    return result;
  }

 private:
  std::vector<int> root_;
  std::vector<int> comp_size_;
  int count_groups_;
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

  if (n > 10000 || q > 50000) {
    std::cout << "skip N > 10000 or Q > 50000\n";
    return 0;
  }

  library3372::internal::Timer timer;
  NaiveUnionFind uf(n);
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
