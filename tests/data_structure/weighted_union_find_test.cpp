// Repository: Library3372
// File: tests/data_structure/weighted_union_find_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/data_structure/weighted_union_find_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include "library3372/data_structure/weighted_union_find.hpp"

#include <algorithm>
#include <cassert>
#include <climits>
#include <cmath>
#include <optional>
#include <random>
#include <type_traits>
#include <utility>
#include <vector>

#include "library3372/math/dynamic_modint.hpp"
#include "library3372/math/static_modint.hpp"

namespace {

using library3372::DynamicModInt;
using library3372::StaticModInt;
using library3372::WeightedUnionFind;

struct Vec2 {
  long long x = 0;
  long long y = 0;

  Vec2& operator+=(const Vec2& rhs) {
    x += rhs.x;
    y += rhs.y;
    return *this;
  }

  Vec2& operator-=(const Vec2& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
  }

  friend bool operator==(const Vec2&, const Vec2&) = default;
};

struct MissingSub {
  MissingSub& operator+=(const MissingSub&) { return *this; }
};

struct NoDefaultGroup {
  explicit NoDefaultGroup(int) {}
  NoDefaultGroup& operator+=(const NoDefaultGroup&) { return *this; }
  NoDefaultGroup& operator-=(const NoDefaultGroup&) { return *this; }
};

template <typename T>
concept CanInstantiateWuf = requires { typename WeightedUnionFind<T>; };

static_assert(CanInstantiateWuf<int>);
static_assert(CanInstantiateWuf<Vec2>);
static_assert(!CanInstantiateWuf<bool>);
static_assert(!CanInstantiateWuf<const int>);
static_assert(!CanInstantiateWuf<int&>);
static_assert(!CanInstantiateWuf<MissingSub>);
static_assert(!CanInstantiateWuf<NoDefaultGroup>);

template <typename T> class NaiveWeightedUnionFind {
 public:
  using UniteResult = typename WeightedUnionFind<T>::UniteResult;

  explicit NaiveWeightedUnionFind(int n)
      : root_(n), pot_(n, T{}), comp_size_(n, 1), count_groups_(n) {
    for (int i = 0; i < n; ++i) root_[i] = i;
  }

  int find(int x) const { return root_[x]; }

  UniteResult unite(int x, int y, const T& w) {
    int rx = root_[x];
    int ry = root_[y];
    if (rx == ry) {
      T cur = pot_[y];
      cur -= pot_[x];
      return {cur == w, std::nullopt};
    }
    T delta = pot_[x];
    delta += w;
    delta -= pot_[y];
    if (comp_size_[rx] < comp_size_[ry]) {
      std::swap(rx, ry);
      T neg{};
      neg -= delta;
      delta = std::move(neg);
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

  T diff(int x, int y) const {
    T res = pot_[y];
    res -= pot_[x];
    return res;
  }

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
  std::vector<T> pot_;
  std::vector<int> comp_size_;
  int count_groups_;
};

template <typename T>
void assert_unite_result_eq(
    const typename WeightedUnionFind<T>::UniteResult& lhs,
    const typename WeightedUnionFind<T>::UniteResult& rhs) {
  assert(lhs.consistent == rhs.consistent);
  assert(lhs.merged == rhs.merged);
}

template <typename T>
void assert_all_queries_match(WeightedUnionFind<T>& uf,
                              const NaiveWeightedUnionFind<T>& naive, int n) {
  assert(uf.count_groups() == naive.count_groups());
  assert(uf.groups() == naive.groups());
  for (int i = 0; i < n; ++i) {
    assert(uf.find(i) == naive.find(i));
    assert(uf.size(i) == naive.size(i));
    for (int j = 0; j < n; ++j) {
      bool s = naive.same(i, j);
      assert(uf.same(i, j) == s);
      if (s) assert(uf.diff(i, j) == naive.diff(i, j));
    }
  }
}

void test_empty_and_single() {
  WeightedUnionFind<long long> uf_default;
  assert(uf_default.count_groups() == 0);
  assert(uf_default.groups().empty());

  WeightedUnionFind<long long> uf0(0);
  assert(uf0.count_groups() == 0);
  assert(uf0.groups().empty());

  WeightedUnionFind<long long> uf1(1);
  assert(uf1.count_groups() == 1);
  assert(uf1.find(0) == 0);
  assert(uf1.same(0, 0));
  assert(uf1.diff(0, 0) == 0);
  assert(uf1.size(0) == 1);
  auto r_ok = uf1.unite(0, 0, 0);
  assert(r_ok.consistent);
  assert(!r_ok.merged.has_value());
  auto r_ng = uf1.unite(0, 0, 1);
  assert(!r_ng.consistent);
  assert(!r_ng.merged.has_value());
  assert(uf1.count_groups() == 1);
  assert((uf1.groups() == std::vector<std::vector<int>>{{0}}));
}

void test_signed_wrap_safety() {
  {
    WeightedUnionFind<int> uf(4);
    auto r0 = uf.unite(0, 1, 1500000000);
    assert(r0.consistent && r0.merged == (std::pair<int, int>{0, 1}));
    auto r1 = uf.unite(1, 2, 1500000000);
    assert(r1.consistent && r1.merged == (std::pair<int, int>{0, 2}));
    int expected02 = (int)(1500000000U + 1500000000U);
    assert(uf.diff(0, 2) == expected02);
    assert(uf.diff(2, 0) == (int)(0U - (unsigned int)expected02));
    auto r2 = uf.unite(0, 2, expected02);
    assert(r2.consistent && !r2.merged.has_value());
    auto r3 = uf.unite(0, 2, expected02 + 1);
    assert(!r3.consistent && !r3.merged.has_value());
    auto r4 = uf.unite(2, 3, -2000000000);
    assert(r4.consistent && r4.merged.has_value());
    int expected03 =
        (int)((unsigned int)expected02 + (unsigned int)-2000000000);
    assert(uf.diff(0, 3) == expected03);
  }
  {
    WeightedUnionFind<long long> uf(4);
    assert(uf.unite(0, 1, LLONG_MAX).consistent);
    assert(uf.unite(1, 2, LLONG_MAX).consistent);
    unsigned long long u02 =
        (unsigned long long)LLONG_MAX + (unsigned long long)LLONG_MAX;
    assert(uf.diff(0, 2) == (long long)u02);
    assert(uf.unite(2, 3, LLONG_MIN).consistent);
    unsigned long long u03 = u02 + (unsigned long long)LLONG_MIN;
    assert(uf.diff(0, 3) == (long long)u03);
    assert(uf.unite(0, 3, (long long)u03).consistent);
    assert(!uf.unite(0, 3, (long long)(u03 + 1ULL)).consistent);
  }
  {
    WeightedUnionFind<signed char> uf8(3);
    assert(uf8.unite(0, 1, (signed char)100).consistent);
    assert(uf8.unite(1, 2, (signed char)100).consistent);
    assert(uf8.diff(0, 2) == (signed char)(unsigned char)200);
    assert(uf8.unite(0, 2, (signed char)(unsigned char)200).consistent);
    assert(!uf8.unite(0, 2, (signed char)0).consistent);
  }
  {
    WeightedUnionFind<short> uf16(3);
    assert(uf16.unite(0, 1, (short)30000).consistent);
    assert(uf16.unite(1, 2, (short)30000).consistent);
    assert(uf16.diff(0, 2) == (short)(unsigned short)60000);
  }
  {
    __int128 half = (__int128)1 << 126;
    WeightedUnionFind<__int128> uf128(3);
    assert(uf128.unite(0, 1, half).consistent);
    assert(uf128.unite(1, 2, half).consistent);
    unsigned __int128 uhalf = (unsigned __int128)half;
    assert(uf128.diff(0, 2) == (__int128)(uhalf * 2));
    assert(uf128.unite(0, 2, (__int128)(uhalf * 2)).consistent);
  }
}

template <typename T>
void test_integer_type(std::mt19937_64& rng, int n, int ops) {
  WeightedUnionFind<T> uf(n);
  NaiveWeightedUnionFind<T> naive(n);
  std::uniform_int_distribution<int> vdist(0, n - 1);
  std::uniform_int_distribution<int> wdist(-5, 5);
  std::uniform_int_distribution<int> opdist(0, 11);

  for (int step = 0; step < ops; ++step) {
    int kind = opdist(rng);
    int u = vdist(rng);
    int v = vdist(rng);
    if (kind < 6) {
      T w = (T)wdist(rng);
      if (naive.same(u, v) && (rng() % 2 == 0)) w = naive.diff(u, v);
      assert_unite_result_eq<T>(uf.unite(u, v, w), naive.unite(u, v, w));
      assert(uf.count_groups() == naive.count_groups());
    } else if (kind < 8) {
      assert(uf.find(u) == naive.find(u));
    } else if (kind < 9) {
      assert(uf.same(u, v) == naive.same(u, v));
      if (naive.same(u, v)) assert(uf.diff(u, v) == naive.diff(u, v));
    } else if (kind < 10) {
      assert(uf.size(u) == naive.size(u));
    } else if (kind < 11) {
      assert(uf.groups() == naive.groups());
    } else {
      assert(uf.count_groups() == naive.count_groups());
    }
  }
  assert_all_queries_match(uf, naive, n);
}

template <typename T> void test_float_type() {
  WeightedUnionFind<T> uf(4);
  auto r1 = uf.unite(0, 1, (T)1.5);
  assert(r1.consistent && r1.merged.has_value());
  auto r2 = uf.unite(2, 3, (T)0.25);
  assert(r2.consistent && r2.merged.has_value());
  auto r3 = uf.unite(1, 2, (T)-2.0);
  assert(r3.consistent && r3.merged.has_value());
  assert(std::fabs((double)(uf.diff(0, 3) - (T)-0.25)) < 1e-6);
  assert(std::fabs((double)(uf.diff(3, 0) - (T)0.25)) < 1e-6);
  auto r4 = uf.unite(0, 3, (T)-0.25);
  assert(r4.consistent && !r4.merged.has_value());
  auto r5 = uf.unite(0, 3, (T)0.5);
  assert(!r5.consistent && !r5.merged.has_value());
}

void test_modint_and_custom_struct(std::mt19937_64& rng) {
  {
    using Mint = StaticModInt<998244353>;
    int n = 30;
    WeightedUnionFind<Mint> uf(n);
    NaiveWeightedUnionFind<Mint> naive(n);
    std::uniform_int_distribution<int> vdist(0, n - 1);
    std::uniform_int_distribution<int> wdist(0, 998244352);
    for (int step = 0; step < 120; ++step) {
      int u = vdist(rng);
      int v = vdist(rng);
      Mint w = (naive.same(u, v) && step % 3 == 0) ? naive.diff(u, v)
                                                   : Mint(wdist(rng));
      assert_unite_result_eq<Mint>(uf.unite(u, v, w), naive.unite(u, v, w));
    }
    assert_all_queries_match(uf, naive, n);
  }
  {
    using Mint1 = StaticModInt<1>;
    WeightedUnionFind<Mint1> uf(5);
    assert(uf.unite(0, 1, 10).consistent);
    assert(uf.unite(0, 1, 20).consistent);
    assert(uf.diff(0, 1) == 0);
  }
  {
    using DMint = DynamicModInt<0>;
    DMint::set_mod(1000000007);
    WeightedUnionFind<DMint> uf(4);
    assert(uf.unite(0, 1, 1000000000).consistent);
    assert(uf.unite(1, 2, 10).consistent);
    assert(uf.diff(0, 2) == 3);
    assert(!uf.unite(0, 2, 4).consistent);
  }
  {
    int n = 25;
    WeightedUnionFind<Vec2> uf(n);
    NaiveWeightedUnionFind<Vec2> naive(n);
    std::uniform_int_distribution<int> vdist(0, n - 1);
    std::uniform_int_distribution<long long> wdist(-1000, 1000);
    for (int step = 0; step < 100; ++step) {
      int u = vdist(rng);
      int v = vdist(rng);
      Vec2 w = (naive.same(u, v) && step % 2 == 0)
                   ? naive.diff(u, v)
                   : Vec2{wdist(rng), wdist(rng)};
      assert_unite_result_eq<Vec2>(uf.unite(u, v, w), naive.unite(u, v, w));
    }
    assert_all_queries_match(uf, naive, n);
  }
}

void test_special_shapes(std::mt19937_64& rng) {
  {
    constexpr int n = 200;
    WeightedUnionFind<long long> uf(n);
    NaiveWeightedUnionFind<long long> naive(n);
    std::uniform_int_distribution<long long> wdist(-1000, 1000);
    for (int i = 0; i + 1 < n; ++i) {
      long long w = wdist(rng);
      assert_unite_result_eq<long long>(uf.unite(i, i + 1, w),
                                        naive.unite(i, i + 1, w));
      assert(uf.count_groups() == naive.count_groups());
    }
    assert_all_queries_match(uf, naive, n);
  }
  {
    constexpr int n = 200;
    WeightedUnionFind<long long> uf(n);
    NaiveWeightedUnionFind<long long> naive(n);
    std::uniform_int_distribution<long long> wdist(-1000, 1000);
    for (int i = 1; i < n; ++i) {
      long long w = wdist(rng);
      assert_unite_result_eq<long long>(uf.unite(0, i, w),
                                        naive.unite(0, i, w));
      assert(uf.count_groups() == naive.count_groups());
    }
    assert_all_queries_match(uf, naive, n);
  }
  {
    // Equal-size pairwise merges forming depth log2(n) before compression.
    constexpr int n = 256;
    WeightedUnionFind<long long> uf(n);
    NaiveWeightedUnionFind<long long> naive(n);
    std::uniform_int_distribution<long long> wdist(-1000, 1000);
    for (int step = 1; step < n; step *= 2) {
      for (int i = 0; i < n; i += 2 * step) {
        long long w = wdist(rng);
        assert_unite_result_eq<long long>(uf.unite(i, i + step, w),
                                          naive.unite(i, i + step, w));
      }
    }
    assert_all_queries_match(uf, naive, n);
  }
}

}  // namespace

int main() {
  std::mt19937_64 rng(0xc3a5c85c97cb3127ULL);

  test_empty_and_single();
  test_signed_wrap_safety();
  test_special_shapes(rng);

  test_integer_type<signed char>(rng, 20, 80);
  test_integer_type<unsigned char>(rng, 20, 80);
  test_integer_type<char>(rng, 20, 80);
  test_integer_type<short>(rng, 30, 120);
  test_integer_type<unsigned short>(rng, 30, 120);
  test_integer_type<int>(rng, 50, 200);
  test_integer_type<unsigned int>(rng, 50, 200);
  test_integer_type<long>(rng, 50, 200);
  test_integer_type<unsigned long>(rng, 50, 200);
  test_integer_type<long long>(rng, 50, 200);
  test_integer_type<unsigned long long>(rng, 50, 200);
  test_integer_type<__int128>(rng, 30, 120);
  test_integer_type<unsigned __int128>(rng, 30, 120);

  test_float_type<float>();
  test_float_type<double>();
  test_float_type<long double>();
  test_float_type<__float128>();

  test_modint_and_custom_struct(rng);

  for (int n : {1, 2, 5, 15, 50, 150}) {
    for (int trial = 0; trial < 4; ++trial) {
      test_integer_type<long long>(rng, n, n * 4);
    }
  }
  return 0;
}
