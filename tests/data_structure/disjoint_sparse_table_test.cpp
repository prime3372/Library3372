// Repository: Library3372
// File: tests/data_structure/disjoint_sparse_table_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/data_structure/disjoint_sparse_table_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include "library3372/data_structure/disjoint_sparse_table.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <climits>
#include <functional>
#include <limits>
#include <random>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "library3372/math/static_modint.hpp"

namespace {

using library3372::DisjointSparseTable;
using library3372::StaticModInt;

using Mint = StaticModInt<998244353>;

struct Affine {
  Mint a;
  Mint b;

  friend bool operator==(const Affine&, const Affine&) = default;
};

Affine affine_op_val(Affine f, Affine g) {
  // Composition g(f(x)) = g.a * (f.a * x + f.b) + g.b
  return {g.a * f.a, g.a * f.b + g.b};
}

Affine affine_op_ref(const Affine& f, const Affine& g) {
  return {g.a * f.a, g.a * f.b + g.b};
}

Affine affine_e() { return {Mint(1), Mint(0)}; }

struct NoDefault {
  long long v;

  explicit NoDefault(long long x) : v(x) {}

  friend bool operator==(const NoDefault&, const NoDefault&) = default;
};

NoDefault nodef_op(const NoDefault& a, const NoDefault& b) {
  return NoDefault(a.v ^ b.v);
}

NoDefault nodef_e() { return NoDefault(0); }

struct Mat2 {
  std::array<unsigned int, 4> m;

  friend bool operator==(const Mat2&, const Mat2&) = default;
};

Mat2 mat_op(const Mat2& lhs, const Mat2& rhs) {
  return {{
      lhs.m[0] * rhs.m[0] + lhs.m[1] * rhs.m[2],
      lhs.m[0] * rhs.m[1] + lhs.m[1] * rhs.m[3],
      lhs.m[2] * rhs.m[0] + lhs.m[3] * rhs.m[2],
      lhs.m[2] * rhs.m[1] + lhs.m[3] * rhs.m[3],
  }};
}

Mat2 mat_e() { return {{1, 0, 0, 1}}; }

struct CountStats {
  inline static int op_calls = 0;
  inline static int e_calls = 0;

  static void reset() {
    op_calls = 0;
    e_calls = 0;
  }
};

long long counted_op(long long a, long long b) {
  ++CountStats::op_calls;
  return a + b;
}

long long counted_e() {
  ++CountStats::e_calls;
  return 0;
}

struct NonConstCallable {
  int operator()(const int& a, const int& b) { return a + b; }
};

struct NonCopyable {
  int x = 0;
  NonCopyable() = default;
  NonCopyable(const NonCopyable&) = delete;
  NonCopyable& operator=(const NonCopyable&) = delete;
  NonCopyable(NonCopyable&&) = default;
  NonCopyable& operator=(NonCopyable&&) = default;
};

int bad_op_mut(int& a, int& b) { return a + b; }
int bad_e_ret() { return 0; }
void bad_e_void() {}

template <typename S, auto op, auto e>
concept CanInstantiateDst =
    requires { typename DisjointSparseTable<S, op, e>; };

static_assert(CanInstantiateDst<Affine, affine_op_val, affine_e>);
static_assert(CanInstantiateDst<Affine, affine_op_ref, affine_e>);
static_assert(CanInstantiateDst<int, std::plus<int>{}, []() { return 0; }>);
static_assert(CanInstantiateDst<NoDefault, nodef_op, nodef_e>);
static_assert(CanInstantiateDst<bool, [](bool a, bool b) { return a && b; },
                                []() { return true; }>);
static_assert(!CanInstantiateDst<const int, [](int a, int b) { return a + b; },
                                 []() { return 0; }>);
static_assert(!CanInstantiateDst<int&, [](int a, int b) { return a + b; },
                                 []() { return 0; }>);
static_assert(!CanInstantiateDst<NonCopyable,
                                 [](const NonCopyable&, const NonCopyable&) {
                                   return NonCopyable{};
                                 },
                                 []() { return NonCopyable{}; }>);
static_assert(!CanInstantiateDst<int, NonConstCallable{}, bad_e_ret>);
static_assert(!CanInstantiateDst<int, bad_op_mut, bad_e_ret>);
static_assert(!CanInstantiateDst<int, std::plus<int>{}, bad_e_void>);
static_assert(!CanInstantiateDst<int, (int (*)(int, int)) nullptr, bad_e_ret>);
static_assert(!CanInstantiateDst<int, std::plus<int>{}, (int (*)()) nullptr>);

template <typename S, auto op, auto e>
void verify_all_ranges(const std::vector<S>& a) {
  const DisjointSparseTable<S, op, e> dst(a);
  int n = (int)a.size();
  assert(dst.size() == n);
  for (int i = 0; i < n; ++i) assert(dst.get(i) == a[i]);

  S total = (S)e();
  for (int i = 0; i < n; ++i) total = (S)op(total, a[i]);
  assert(dst.all_prod() == total);

  for (int l = 0; l <= n; ++l) {
    S acc = (S)e();
    for (int r = l; r <= n; ++r) {
      assert(dst.prod(l, r) == acc);
      if (r < n) acc = (S)op(acc, a[r]);
    }
  }
}

void test_empty_and_small_exhaustive(std::mt19937_64& rng) {
  {
    const DisjointSparseTable<Affine, affine_op_val, affine_e> empty_dst;
    assert(empty_dst.size() == 0);
    assert(empty_dst.prod(0, 0) == affine_e());
    assert(empty_dst.all_prod() == affine_e());
  }

  std::uniform_int_distribution<int> val_dist(1, 998244352);
  for (int n = 0; n <= 33; ++n) {
    std::vector<Affine> a(n);
    for (int i = 0; i < n; ++i) {
      a[i] = {Mint(val_dist(rng)), Mint(val_dist(rng))};
    }
    verify_all_ranges<Affine, affine_op_val, affine_e>(a);
    verify_all_ranges<Affine, affine_op_ref, affine_e>(a);
  }
}

void test_callable_variants() {
  std::vector<long long> v = {3, -1, 4, 1, -5, 9, 2, -6};

  // Generic lambda and int-returning e() for long long S
  constexpr auto gen_op = [](const auto& a, const auto& b) {
    return std::max<long long>(a, b);
  };
  constexpr auto int_e = []() { return INT_MIN; };
  verify_all_ranges<long long, gen_op, int_e>(v);

  // Standard library functor
  DisjointSparseTable<long long, std::plus<long long>{}, []() { return 0; }>
      dst_plus(v);
  assert(dst_plus.prod(0, 8) == 7);
  assert(dst_plus.prod(2, 6) == 9);
  assert(dst_plus.all_prod() == 7);

  // Wide return type from op into narrower S
  std::vector<int> vi = {1, 2, 3, 4, 5, 6};
  constexpr auto wide_op = [](int a, int b) -> long long {
    return (long long)a + b;
  };
  verify_all_ranges<int, wide_op, []() -> long long { return 0LL; }>(vi);
}

void test_non_commutative_and_non_default_constructible() {
  std::vector<std::string> words = {"a", "bc", "def", "g", "hi", "jklm", "no"};
  constexpr auto str_op = [](const std::string& a, const std::string& b) {
    return a + b;
  };
  constexpr auto str_e = []() { return std::string(); };
  verify_all_ranges<std::string, str_op, str_e>(words);

  std::vector<NoDefault> nd;
  for (int i = 1; i <= 17; ++i) nd.emplace_back(i * 7 - 3);
  verify_all_ranges<NoDefault, nodef_op, nodef_e>(nd);
}

void test_bool_monoid() {
  std::vector<bool> b1 = {true, true, false, true, true, true, false, true};
  verify_all_ranges<bool, [](bool a, bool b) { return a && b; },
                    []() { return true; }>(b1);
  verify_all_ranges<bool, [](bool a, bool b) { return a || b; },
                    []() { return false; }>(b1);
  verify_all_ranges<bool, [](bool a, bool b) { return a != b; },
                    []() { return false; }>(b1);
}

void test_query_operation_counts() {
  std::vector<long long> v = {10, 20, 30, 40, 50, 60, 70, 80, 90};
  DisjointSparseTable<long long, counted_op, counted_e> dst(v);

  CountStats::reset();
  assert(dst.get(4) == 50);
  assert(CountStats::op_calls == 0);
  assert(CountStats::e_calls == 0);

  CountStats::reset();
  assert(dst.prod(3, 3) == 0);
  assert(CountStats::op_calls == 0);
  assert(CountStats::e_calls == 1);

  CountStats::reset();
  assert(dst.prod(3, 4) == 40);
  assert(CountStats::op_calls == 0);
  assert(CountStats::e_calls == 0);

  CountStats::reset();
  assert(dst.prod(2, 7) == 250);
  assert(CountStats::op_calls == 1);
  assert(CountStats::e_calls == 0);

  CountStats::reset();
  assert(dst.all_prod() == 450);
  assert(CountStats::op_calls == 1);
  assert(CountStats::e_calls == 0);
}

void test_copy_and_move() {
  std::vector<int> v = {5, 3, 8, 1, 9, 2};
  constexpr auto min_op = [](int a, int b) { return std::min(a, b); };
  constexpr auto min_e = []() { return std::numeric_limits<int>::max(); };
  using DstMin = DisjointSparseTable<int, min_op, min_e>;

  DstMin original(v);
  DstMin copy_ctor(original);
  DstMin copy_assign;
  copy_assign = original;

  assert(copy_ctor.size() == 6);
  assert(copy_assign.size() == 6);
  assert(copy_ctor.prod(1, 5) == 1);
  assert(copy_assign.prod(0, 3) == 3);

  DstMin moved_ctor(std::move(copy_ctor));
  assert(moved_ctor.size() == 6);
  assert(moved_ctor.prod(1, 5) == 1);
  // Moved-from object must be safely reassignable and destructible.
  copy_ctor = original;
  assert(copy_ctor.prod(0, 6) == 1);

  DstMin moved_assign;
  moved_assign = std::move(copy_assign);
  assert(moved_assign.size() == 6);
  assert(moved_assign.prod(2, 6) == 1);
  copy_assign = DstMin();
  assert(copy_assign.size() == 0);
  assert(copy_assign.all_prod() == std::numeric_limits<int>::max());
}

void test_random_large(std::mt19937_64& rng) {
  std::uniform_int_distribution<unsigned int> u32_dist;
  for (int n : {63, 64, 65, 255, 256, 257, 1023, 1024, 1025}) {
    std::vector<Mat2> a(n);
    for (int i = 0; i < n; ++i) {
      a[i] = {{u32_dist(rng), u32_dist(rng), u32_dist(rng), u32_dist(rng)}};
    }
    DisjointSparseTable<Mat2, mat_op, mat_e> dst(a);

    std::vector<Mat2> pref(n + 1, mat_e());
    for (int i = 0; i < n; ++i) pref[i + 1] = mat_op(pref[i], a[i]);
    // Note: Mat2 isn't invertible, so verify against naive for sampled queries.
    std::uniform_int_distribution<int> bound_dist(0, n);
    for (int q = 0; q < 200; ++q) {
      int l = bound_dist(rng);
      int r = bound_dist(rng);
      if (l > r) std::swap(l, r);
      Mat2 expected = mat_e();
      for (int i = l; i < r; ++i) expected = mat_op(expected, a[i]);
      assert(dst.prod(l, r) == expected);
    }
    assert(dst.all_prod() == pref[n]);
  }
}

}  // namespace

int main() {
  std::mt19937_64 rng(0x9e3779b97f4a7c15ULL);

  test_empty_and_small_exhaustive(rng);
  test_callable_variants();
  test_non_commutative_and_non_default_constructible();
  test_bool_monoid();
  test_query_operation_counts();
  test_copy_and_move();
  test_random_large(rng);

  return 0;
}
