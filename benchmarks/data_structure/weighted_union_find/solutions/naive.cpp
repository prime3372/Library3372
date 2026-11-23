// Repository: Library3372
// File: benchmarks/data_structure/weighted_union_find/solutions/naive.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/data_structure/weighted_union_find/solutions/naive.cpp
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
  long long w;
};

class NaiveWeightedUnionFind {
 public:
  struct UniteResult {
    bool consistent;
    std::optional<std::pair<int, int>> merged;
  };

  explicit NaiveWeightedUnionFind(int n)
      : root_(n), pot_(n, 0ULL), comp_size_(n, 1), count_groups_(n) {
    for (int i = 0; i < n; ++i) root_[i] = i;
  }

  int find(int x) const { return root_[x]; }

  UniteResult unite(int x, int y, long long w) {
    int rx = root_[x];
    int ry = root_[y];
    unsigned long long uw = (unsigned long long)w;
    if (rx == ry) {
      return {(pot_[y] - pot_[x]) == uw, std::nullopt};
    }
    unsigned long long delta = pot_[x] + uw - pot_[y];
    if (comp_size_[rx] < comp_size_[ry]) {
      std::swap(rx, ry);
      delta = 0ULL - delta;
    }
    for (int i = 0; i < (int)root_.size(); ++i) {
      if (root_[i] == ry) {
        root_[i] = rx;
        pot_[i] += delta;
      }
    }
    comp_size_[rx] += comp_size_[ry];
    comp_size_[ry] = 0;
    --count_groups_;
    return {true, std::pair<int, int>{rx, ry}};
  }

  bool same(int x, int y) const { return root_[x] == root_[y]; }

  long long diff(int x, int y) const { return (long long)(pot_[y] - pot_[x]); }

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
  std::vector<unsigned long long> pot_;
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
    if (t == 0) {
      std::cin >> queries[i].u >> queries[i].v >> queries[i].w;
    } else if (t == 1 || t == 2) {
      std::cin >> queries[i].u >> queries[i].v;
    } else if (t == 3 || t == 4) {
      std::cin >> queries[i].u;
    }
  }

  if (n > 10000 || q > 50000) {
    std::cout << "skip N > 10000 or Q > 50000\n";
    return 0;
  }

  library3372::internal::Timer timer;
  NaiveWeightedUnionFind uf(n);
  double init_sec = timer.elapsed();

  timer.reset();
  unsigned long long ans_hash = 0;
  for (const auto& qry : queries) {
    if (qry.type == 0) {
      auto res = uf.unite(qry.u, qry.v, qry.w);
      ans_hash = library3372::internal::hash_combine(
          ans_hash, (unsigned long long)res.consistent);
      if (res.merged.has_value()) {
        ans_hash = library3372::internal::hash_combine(
            ans_hash, (unsigned long long)res.merged->first);
        ans_hash = library3372::internal::hash_combine(
            ans_hash, (unsigned long long)res.merged->second);
      } else {
        ans_hash = library3372::internal::hash_combine(ans_hash, 0ULL);
      }
    } else if (qry.type == 1) {
      ans_hash = library3372::internal::hash_combine(
          ans_hash, (unsigned long long)uf.same(qry.u, qry.v));
    } else if (qry.type == 2) {
      unsigned long long d = uf.same(qry.u, qry.v)
                                 ? (unsigned long long)uf.diff(qry.u, qry.v)
                                 : 0ULL;
      ans_hash = library3372::internal::hash_combine(ans_hash, d);
    } else if (qry.type == 3) {
      ans_hash = library3372::internal::hash_combine(
          ans_hash, (unsigned long long)uf.size(qry.u));
    } else if (qry.type == 4) {
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
