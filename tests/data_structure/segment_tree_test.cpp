// Repository: Library3372
// File: tests/data_structure/segment_tree_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/data_structure/segment_tree_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include "library3372/data_structure/segment_tree.hpp"

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

using library3372::SegmentTree;
using library3372::StaticModInt;

using Mint = StaticModInt<998244353>;

struct Affine {
  Mint a;
  Mint b;

  friend bool operator==(const Affine&, const Affine&) = default;
};

Affine affine_op_val(Affine f, Affine g) {
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

bool pred_val(int x) { return x < 5; }
bool pred_ref(const int& x) { return x < 5; }
bool bad_pred_mut(int& x) { return x < 5; }
void bad_pred_void(int) {}

struct PredFunctor {
  int limit;
  bool operator()(const int& x) const { return x <= limit; }
};

template <typename S, auto op, auto e>
concept CanInstantiateSeg = requires { typename SegmentTree<S, op, e>; };

template <typename F>
concept CanMaxRight =
    requires(const SegmentTree<int, std::plus<int>{}, []() { return 0; }>& seg,
             F f) { seg.max_right(0, f); };

template <typename F>
concept CanMinLeft =
    requires(const SegmentTree<int, std::plus<int>{}, []() { return 0; }>& seg,
             F f) { seg.min_left(0, f); };

static_assert(CanInstantiateSeg<Affine, affine_op_val, affine_e>);
static_assert(CanInstantiateSeg<Affine, affine_op_ref, affine_e>);
static_assert(CanInstantiateSeg<int, std::plus<int>{}, []() { return 0; }>);
static_assert(CanInstantiateSeg<NoDefault, nodef_op, nodef_e>);
static_assert(CanInstantiateSeg<bool, [](bool a, bool b) { return a && b; },
                                []() { return true; }>);
static_assert(!CanInstantiateSeg<const int, [](int a, int b) { return a + b; },
                                 []() { return 0; }>);
static_assert(!CanInstantiateSeg<int&, [](int a, int b) { return a + b; },
                                 []() { return 0; }>);
static_assert(!CanInstantiateSeg<NonCopyable,
                                 [](const NonCopyable&, const NonCopyable&) {
                                   return NonCopyable{};
                                 },
                                 []() { return NonCopyable{}; }>);
static_assert(!CanInstantiateSeg<int, NonConstCallable{}, bad_e_ret>);
static_assert(!CanInstantiateSeg<int, bad_op_mut, bad_e_ret>);
static_assert(!CanInstantiateSeg<int, std::plus<int>{}, bad_e_void>);
static_assert(!CanInstantiateSeg<int, (int (*)(int, int)) nullptr, bad_e_ret>);
static_assert(!CanInstantiateSeg<int, std::plus<int>{}, (int (*)()) nullptr>);

static_assert(CanMaxRight<decltype(&pred_val)>);
static_assert(CanMaxRight<decltype(&pred_ref)>);
static_assert(CanMaxRight<PredFunctor>);
static_assert(!CanMaxRight<decltype(&bad_pred_mut)>);
static_assert(!CanMaxRight<decltype(&bad_pred_void)>);
static_assert(CanMinLeft<decltype(&pred_val)>);
static_assert(CanMinLeft<decltype(&pred_ref)>);
static_assert(CanMinLeft<PredFunctor>);
static_assert(!CanMinLeft<decltype(&bad_pred_mut)>);
static_assert(!CanMinLeft<decltype(&bad_pred_void)>);

template <typename S, auto op, auto e>
void verify_all_ranges(const SegmentTree<S, op, e>& seg,
                       const std::vector<S>& a) {
  int n = (int)a.size();
  assert(seg.size() == n);
  for (int i = 0; i < n; ++i) assert(seg.get(i) == a[i]);

  S total = (S)e();
  for (int i = 0; i < n; ++i) total = (S)op(total, a[i]);
  assert(seg.all_prod() == total);

  for (int l = 0; l <= n; ++l) {
    S acc = (S)e();
    for (int r = l; r <= n; ++r) {
      assert(seg.prod(l, r) == acc);
      if (r < n) acc = (S)op(acc, a[r]);
    }
  }
}

void test_empty_and_small_exhaustive(std::mt19937_64& rng) {
  {
    const SegmentTree<Affine, affine_op_val, affine_e> empty_default;
    assert(empty_default.size() == 0);
    assert(empty_default.prod(0, 0) == affine_e());
    assert(empty_default.all_prod() == affine_e());
    assert(empty_default.max_right(0, [](const Affine&) { return true; }) == 0);
    assert(empty_default.min_left(0, [](const Affine&) { return true; }) == 0);

    const SegmentTree<Affine, affine_op_val, affine_e> empty_n(0);
    assert(empty_n.size() == 0);
    assert(empty_n.prod(0, 0) == affine_e());
    assert(empty_n.all_prod() == affine_e());
  }

  std::uniform_int_distribution<int> val_dist(1, 998244352);
  for (int n = 0; n <= 33; ++n) {
    std::vector<Affine> a(n);
    for (int i = 0; i < n; ++i) {
      a[i] = {Mint(val_dist(rng)), Mint(val_dist(rng))};
    }

    SegmentTree<Affine, affine_op_val, affine_e> seg_vec(a);
    verify_all_ranges(seg_vec, a);

    SegmentTree<Affine, affine_op_ref, affine_e> seg_n(n);
    std::vector<Affine> cur(n, affine_e());
    verify_all_ranges(seg_n, cur);
    for (int i = 0; i < n; ++i) {
      cur[i] = a[i];
      seg_n.set(i, a[i]);
      if (i % 5 == 0 || i + 1 == n) verify_all_ranges(seg_n, cur);
    }
    for (int step = 0; step < 5 && n > 0; ++step) {
      int p = (int)(rng() % (unsigned int)n);
      Affine nv = {Mint(val_dist(rng)), Mint(val_dist(rng))};
      cur[p] = nv;
      seg_vec.set(p, nv);
      verify_all_ranges(seg_vec, cur);
    }
  }
}

void test_binary_search_exhaustive(std::mt19937_64& rng) {
  constexpr auto sum_op = [](long long a, long long b) { return a + b; };
  constexpr auto sum_e = []() { return 0LL; };

  std::uniform_int_distribution<long long> val_dist(0, 5);
  for (int n = 0; n <= 33; ++n) {
    std::vector<long long> a(n);
    for (int i = 0; i < n; ++i) a[i] = val_dist(rng);
    SegmentTree<long long, sum_op, sum_e> seg(a);

    auto check_all = [&]() {
      long long total = 0;
      for (long long v : a) total += v;
      for (long long limit = 0; limit <= total + 2; ++limit) {
        auto pred = [limit](long long s) { return s <= limit; };
        for (int l = 0; l <= n; ++l) {
          long long acc = 0;
          int expected_r = l;
          while (expected_r < n && acc + a[expected_r] <= limit) {
            acc += a[expected_r];
            ++expected_r;
          }
          assert(seg.max_right(l, pred) == expected_r);
        }
        for (int r = 0; r <= n; ++r) {
          long long acc = 0;
          int expected_l = r;
          while (expected_l > 0 && acc + a[expected_l - 1] <= limit) {
            acc += a[expected_l - 1];
            --expected_l;
          }
          assert(seg.min_left(r, pred) == expected_l);
        }
      }
    };

    check_all();
    if (n > 0) {
      for (int t = 0; t < 3; ++t) {
        int p = (int)(rng() % (unsigned int)n);
        long long nv = val_dist(rng);
        a[p] = nv;
        seg.set(p, nv);
        check_all();
      }
    }
  }
}

void test_callable_variants() {
  std::vector<long long> v = {3, -1, 4, 1, -5, 9, 2, -6};

  constexpr auto gen_op = [](const auto& a, const auto& b) {
    return std::max<long long>(a, b);
  };
  constexpr auto int_e = []() { return INT_MIN; };
  SegmentTree<long long, gen_op, int_e> seg_max(v);
  verify_all_ranges(seg_max, v);

  SegmentTree<long long, std::plus<long long>{}, []() { return 0; }> seg_plus(
      v);
  assert(seg_plus.prod(0, 8) == 7);
  assert(seg_plus.prod(2, 6) == 9);
  assert(seg_plus.all_prod() == 7);

  std::vector<int> vi = {1, 2, 3, 4, 5, 6};
  constexpr auto wide_op = [](int a, int b) -> long long {
    return (long long)a + b;
  };
  SegmentTree<int, wide_op, []() -> long long { return 0LL; }> seg_wide(vi);
  verify_all_ranges(seg_wide, vi);
  assert(seg_wide.max_right(1, pred_val) == 2);
  assert(seg_wide.max_right(1, pred_ref) == 2);
  assert(seg_wide.max_right(0, PredFunctor{6}) == 3);
  assert(seg_wide.min_left(6, PredFunctor{11}) == 4);
}

void test_non_commutative_and_non_default_constructible() {
  std::vector<std::string> words = {"a", "bc", "def", "g", "hi", "jklm", "no"};
  constexpr auto str_op = [](const std::string& a, const std::string& b) {
    return a + b;
  };
  constexpr auto str_e = []() { return std::string(); };
  SegmentTree<std::string, str_op, str_e> seg_str(words);
  verify_all_ranges(seg_str, words);

  seg_str.set(3, "XYZ");
  words[3] = "XYZ";
  verify_all_ranges(seg_str, words);

  for (int limit = 0; limit <= 20; ++limit) {
    auto len_pred = [limit](const std::string& s) {
      return (int)s.size() <= limit;
    };
    for (int l = 0; l <= (int)words.size(); ++l) {
      std::string acc;
      int expected_r = l;
      while (expected_r < (int)words.size() &&
             (int)(acc + words[expected_r]).size() <= limit) {
        acc += words[expected_r];
        ++expected_r;
      }
      assert(seg_str.max_right(l, len_pred) == expected_r);
    }
    for (int r = 0; r <= (int)words.size(); ++r) {
      std::string acc;
      int expected_l = r;
      while (expected_l > 0 &&
             (int)(words[expected_l - 1] + acc).size() <= limit) {
        acc = words[expected_l - 1] + acc;
        --expected_l;
      }
      assert(seg_str.min_left(r, len_pred) == expected_l);
    }
  }

  // Verify order preservation inside predicate during max_right / min_left.
  for (int l = 0; l <= (int)words.size(); ++l) {
    int r = seg_str.max_right(l, [&](const std::string& s) {
      if (s.empty()) return true;
      std::string full;
      for (int i = l; i < (int)words.size(); ++i) full += words[i];
      return full.starts_with(s) && s.size() <= 6;
    });
    std::string acc;
    int expected_r = l;
    while (expected_r < (int)words.size() &&
           (acc + words[expected_r]).size() <= 6) {
      acc += words[expected_r];
      ++expected_r;
    }
    assert(r == expected_r);
  }
  for (int r = 0; r <= (int)words.size(); ++r) {
    int l = seg_str.min_left(r, [&](const std::string& s) {
      if (s.empty()) return true;
      std::string full;
      for (int i = 0; i < r; ++i) full += words[i];
      return full.ends_with(s) && s.size() <= 6;
    });
    std::string acc;
    int expected_l = r;
    while (expected_l > 0 && (words[expected_l - 1] + acc).size() <= 6) {
      acc = words[expected_l - 1] + acc;
      --expected_l;
    }
    assert(l == expected_l);
  }

  std::vector<NoDefault> nd;
  for (int i = 1; i <= 17; ++i) nd.emplace_back(i * 7 - 3);
  SegmentTree<NoDefault, nodef_op, nodef_e> seg_nd(nd);
  verify_all_ranges(seg_nd, nd);
  seg_nd.set(5, NoDefault(12345));
  nd[5] = NoDefault(12345);
  verify_all_ranges(seg_nd, nd);

  SegmentTree<NoDefault, nodef_op, nodef_e> seg_nd_n(9);
  std::vector<NoDefault> nd_init(9, nodef_e());
  verify_all_ranges(seg_nd_n, nd_init);
}

void test_bool_monoid() {
  std::vector<bool> b1 = {true, true, false, true, true, true, false, true};
  {
    SegmentTree<bool, [](bool a, bool b) { return a && b; },
                []() { return true; }>
        seg_and(b1);
    verify_all_ranges(seg_and, b1);
    assert(seg_and.max_right(0, [](bool x) { return x; }) == 2);
    assert(seg_and.min_left(6, [](bool x) { return x; }) == 3);
    seg_and.set(2, true);
    b1[2] = true;
    verify_all_ranges(seg_and, b1);
    assert(seg_and.max_right(0, [](bool x) { return x; }) == 6);
  }
  {
    SegmentTree<bool, [](bool a, bool b) { return a || b; },
                []() { return false; }>
        seg_or(b1);
    verify_all_ranges(seg_or, b1);
    seg_or.set(0, false);
    b1[0] = false;
    verify_all_ranges(seg_or, b1);
  }
  {
    SegmentTree<bool, [](bool a, bool b) { return a != b; },
                []() { return false; }>
        seg_xor(b1);
    verify_all_ranges(seg_xor, b1);
  }
  {
    SegmentTree<bool, [](bool a, bool b) { return a && b; },
                []() { return true; }>
        seg_n(5);
    std::vector<bool> all_true(5, true);
    verify_all_ranges(seg_n, all_true);
  }
}

void test_operation_counts() {
  CountStats::reset();
  SegmentTree<long long, counted_op, counted_e> seg_n(5);
  assert(CountStats::op_calls == 0);
  assert(CountStats::e_calls == 1);

  std::vector<long long> v = {10, 20, 30, 40, 50, 60, 70, 80, 90};
  CountStats::reset();
  SegmentTree<long long, counted_op, counted_e> seg(v);
  assert(CountStats::op_calls == 11);
  assert(CountStats::e_calls == 1);

  CountStats::reset();
  assert(seg.get(4) == 50);
  assert(CountStats::op_calls == 0);
  assert(CountStats::e_calls == 0);

  CountStats::reset();
  assert(seg.all_prod() == 450);
  assert(CountStats::op_calls == 0);
  assert(CountStats::e_calls == 0);

  CountStats::reset();
  assert(seg.prod(3, 3) == 0);
  assert(CountStats::op_calls == 0);
  assert(CountStats::e_calls == 1);

  CountStats::reset();
  assert(seg.prod(3, 4) == 40);
  assert(CountStats::op_calls == 0);
  assert(CountStats::e_calls == 0);

  CountStats::reset();
  assert(seg.prod(0, 4) == 100);
  assert(CountStats::op_calls == 0);
  assert(CountStats::e_calls == 0);

  CountStats::reset();
  assert(seg.prod(2, 7) == 250);
  assert(CountStats::op_calls == 2);
  assert(CountStats::e_calls == 0);
}

void test_copy_and_move() {
  std::vector<int> v = {5, 3, 8, 1, 9, 2};
  constexpr auto min_op = [](int a, int b) { return std::min(a, b); };
  constexpr auto min_e = []() { return std::numeric_limits<int>::max(); };
  using SegMin = SegmentTree<int, min_op, min_e>;

  SegMin original(v);
  SegMin copy_ctor(original);
  SegMin copy_assign;
  copy_assign = original;

  original.set(3, 100);
  assert(original.prod(1, 5) == 3);
  assert(copy_ctor.size() == 6);
  assert(copy_assign.size() == 6);
  assert(copy_ctor.prod(1, 5) == 1);
  assert(copy_assign.prod(0, 3) == 3);

  SegMin moved_ctor(std::move(copy_ctor));
  assert(moved_ctor.size() == 6);
  assert(moved_ctor.prod(1, 5) == 1);
  copy_ctor = original;
  assert(copy_ctor.prod(0, 6) == 2);

  SegMin moved_assign;
  moved_assign = std::move(copy_assign);
  assert(moved_assign.size() == 6);
  assert(moved_assign.prod(2, 6) == 1);
  copy_assign = SegMin();
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
    SegmentTree<Mat2, mat_op, mat_e> seg(a);

    std::uniform_int_distribution<int> pos_dist(0, n - 1);
    std::uniform_int_distribution<int> bound_dist(0, n);
    for (int q = 0; q < 200; ++q) {
      if (q % 4 == 0) {
        int p = pos_dist(rng);
        Mat2 nv = {
            {u32_dist(rng), u32_dist(rng), u32_dist(rng), u32_dist(rng)}};
        a[p] = nv;
        seg.set(p, nv);
        assert(seg.get(p) == nv);
      } else {
        int l = bound_dist(rng);
        int r = bound_dist(rng);
        if (l > r) std::swap(l, r);
        Mat2 expected = mat_e();
        for (int i = l; i < r; ++i) expected = mat_op(expected, a[i]);
        assert(seg.prod(l, r) == expected);
      }
    }
    Mat2 expected_all = mat_e();
    for (int i = 0; i < n; ++i) expected_all = mat_op(expected_all, a[i]);
    assert(seg.all_prod() == expected_all);
  }
}

void test_random_binary_search_medium(std::mt19937_64& rng) {
  constexpr auto max_op = [](long long a, long long b) {
    return std::max(a, b);
  };
  constexpr auto max_e = []() { return 0LL; };
  std::uniform_int_distribution<long long> val_dist(0, 1000);
  for (int n : {100, 511, 512, 513}) {
    std::vector<long long> a(n);
    for (int i = 0; i < n; ++i) a[i] = val_dist(rng);
    SegmentTree<long long, max_op, max_e> seg(a);
    std::uniform_int_distribution<int> pos_dist(0, n - 1);
    std::uniform_int_distribution<int> bound_dist(0, n);
    for (int q = 0; q < 200; ++q) {
      int kind = (int)(rng() % 3);
      if (kind == 0) {
        int p = pos_dist(rng);
        long long nv = val_dist(rng);
        a[p] = nv;
        seg.set(p, nv);
      } else if (kind == 1) {
        int l = bound_dist(rng);
        long long limit = val_dist(rng);
        int r = l;
        long long cur = 0;
        while (r < n && std::max(cur, a[r]) <= limit) {
          cur = std::max(cur, a[r]);
          ++r;
        }
        assert(seg.max_right(l, [limit](long long x) { return x <= limit; }) ==
               r);
      } else {
        int r = bound_dist(rng);
        long long limit = val_dist(rng);
        int l = r;
        long long cur = 0;
        while (l > 0 && std::max(cur, a[l - 1]) <= limit) {
          cur = std::max(cur, a[l - 1]);
          --l;
        }
        assert(seg.min_left(r, [limit](long long x) { return x <= limit; }) ==
               l);
      }
    }
  }
}

}  // namespace

int main() {
  std::mt19937_64 rng(0x9e3779b97f4a7c15ULL);

  test_empty_and_small_exhaustive(rng);
  test_binary_search_exhaustive(rng);
  test_callable_variants();
  test_non_commutative_and_non_default_constructible();
  test_bool_monoid();
  test_operation_counts();
  test_copy_and_move();
  test_random_large(rng);
  test_random_binary_search_medium(rng);

  return 0;
}
