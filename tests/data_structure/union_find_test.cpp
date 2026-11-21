// Repository: Library3372
// File: tests/data_structure/union_find_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/data_structure/union_find_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include "library3372/data_structure/union_find.hpp"

#include <cassert>
#include <optional>
#include <random>
#include <utility>
#include <vector>

namespace {

using library3372::UnionFind;

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

void assert_all_queries_match(UnionFind& uf, const NaiveUnionFind& naive,
                              int n) {
  assert(uf.count_groups() == naive.count_groups());
  assert(uf.groups() == naive.groups());
  for (int i = 0; i < n; ++i) {
    assert(uf.find(i) == naive.find(i));
    assert(uf.size(i) == naive.size(i));
  }
}

void test_empty_and_single() {
  UnionFind uf_default;
  assert(uf_default.count_groups() == 0);
  assert(uf_default.groups().empty());

  UnionFind uf0(0);
  assert(uf0.count_groups() == 0);
  assert(uf0.groups().empty());

  UnionFind uf1(1);
  assert(uf1.count_groups() == 1);
  assert(uf1.find(0) == 0);
  assert(uf1.same(0, 0));
  assert(uf1.size(0) == 1);
  assert(!uf1.unite(0, 0).has_value());
  assert(uf1.count_groups() == 1);
  assert((uf1.groups() == std::vector<std::vector<int>>{{0}}));
}

void test_random(std::mt19937_64& rng, int n, int ops) {
  UnionFind uf(n);
  NaiveUnionFind naive(n);

  std::uniform_int_distribution<int> vdist(0, n - 1);
  std::uniform_int_distribution<int> opdist(0, 9);

  for (int step = 0; step < ops; ++step) {
    int kind = opdist(rng);
    int u = vdist(rng);
    int v = vdist(rng);
    if (kind < 5) {
      assert(uf.unite(u, v) == naive.unite(u, v));
      assert(uf.count_groups() == naive.count_groups());
    } else if (kind < 7) {
      assert(uf.find(u) == naive.find(u));
    } else if (kind < 8) {
      assert(uf.same(u, v) == naive.same(u, v));
    } else if (kind < 9) {
      assert(uf.size(u) == naive.size(u));
    } else {
      assert(uf.groups() == naive.groups());
    }
  }
  assert_all_queries_match(uf, naive, n);
}

void test_special_shapes() {
  {
    constexpr int n = 1000;
    UnionFind uf(n);
    NaiveUnionFind naive(n);
    for (int i = 0; i + 1 < n; ++i) {
      assert(uf.unite(i, i + 1) == naive.unite(i, i + 1));
      assert(uf.count_groups() == naive.count_groups());
    }
    assert_all_queries_match(uf, naive, n);
  }
  {
    constexpr int n = 1000;
    UnionFind uf(n);
    NaiveUnionFind naive(n);
    for (int i = 1; i < n; ++i) {
      assert(uf.unite(0, i) == naive.unite(0, i));
      assert(uf.count_groups() == naive.count_groups());
    }
    assert_all_queries_match(uf, naive, n);
  }
  {
    // Equal-size pairwise merges forming depth log2(n) before compression.
    constexpr int n = 1024;
    UnionFind uf(n);
    NaiveUnionFind naive(n);
    for (int step = 1; step < n; step *= 2) {
      for (int i = 0; i < n; i += 2 * step) {
        assert(uf.unite(i, i + step) == naive.unite(i, i + step));
      }
    }
    assert_all_queries_match(uf, naive, n);
  }
}

}  // namespace

int main() {
  test_empty_and_single();
  test_special_shapes();

  std::mt19937_64 rng(0x9e3779b97f4a7c15ULL);
  for (int n : {1, 2, 5, 15, 50, 200}) {
    for (int trial = 0; trial < 5; ++trial) {
      test_random(rng, n, n * 4);
    }
  }
  return 0;
}
