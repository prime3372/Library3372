// Repository: Library3372
// File: tests/data_structure/lazy_segment_tree_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/data_structure/lazy_segment_tree_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include "library3372/data_structure/lazy_segment_tree.hpp"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <functional>
#include <limits>
#include <random>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "library3372/math/static_modint.hpp"

namespace {

using library3372::LazySegmentTree;
using library3372::StaticModInt;

using Mint = StaticModInt<998244353>;

struct SumLen {
  Mint sum;
  int len;

  friend bool operator==(const SumLen&, const SumLen&) = default;
};

struct Affine {
  Mint b;
  Mint c;

  friend bool operator==(const Affine&, const Affine&) = default;
};

SumLen sum_op(const SumLen& x, const SumLen& y) {
  return {x.sum + y.sum, x.len + y.len};
}

SumLen sum_e() { return {Mint(0), 0}; }

SumLen aff_mapping_val(Affine f, SumLen x) {
  return {f.b * x.sum + f.c * x.len, x.len};
}

SumLen aff_mapping_ref(const Affine& f, const SumLen& x) {
  return {f.b * x.sum + f.c * x.len, x.len};
}

Affine aff_composition(const Affine& f, const Affine& g) {
  return {f.b * g.b, f.b * g.c + f.c};
}

Affine aff_id() { return {Mint(1), Mint(0)}; }

struct NoDefaultS {
  long long v;
  int parity;

  explicit NoDefaultS(long long x, int p) : v(x), parity(p) {}

  friend bool operator==(const NoDefaultS&, const NoDefaultS&) = default;
};

struct NoDefaultF {
  long long k;

  explicit NoDefaultF(long long x) : k(x) {}

  friend bool operator==(const NoDefaultF&, const NoDefaultF&) = default;
};

NoDefaultS nd_op(const NoDefaultS& a, const NoDefaultS& b) {
  return NoDefaultS(a.v ^ b.v, a.parity ^ b.parity);
}

NoDefaultS nd_e() { return NoDefaultS(0, 0); }

NoDefaultS nd_mapping(const NoDefaultF& f, const NoDefaultS& x) {
  return NoDefaultS(x.parity ? (x.v ^ f.k) : x.v, x.parity);
}

NoDefaultF nd_composition(const NoDefaultF& f, const NoDefaultF& g) {
  return NoDefaultF(f.k ^ g.k);
}

NoDefaultF nd_id() { return NoDefaultF(0); }

struct NonCopyable {
  int x = 0;
  NonCopyable() = default;
  NonCopyable(const NonCopyable&) = delete;
  NonCopyable& operator=(const NonCopyable&) = delete;
  NonCopyable(NonCopyable&&) = default;
  NonCopyable& operator=(NonCopyable&&) = default;
};

struct NonConstCallable {
  int operator()(const int& a, const int& b) { return a + b; }
};

int bad_map_mut(int& f, int& x) { return f + x; }
void bad_map_void(int, int) {}
void bad_id_void() {}

bool pred_val(int x) { return x < 5; }
bool pred_ref(const int& x) { return x < 5; }
bool bad_pred_mut(int& x) { return x < 5; }
void bad_pred_void(int) {}

struct PredFunctor {
  int limit;
  bool operator()(const int& x) const { return x <= limit; }
};

template <typename S, auto op, auto e, typename F, auto mapping,
          auto composition, auto id>
concept CanInstantiateLazy = requires {
  typename LazySegmentTree<S, op, e, F, mapping, composition, id>;
};

constexpr auto int_add = [](int a, int b) { return a + b; };
constexpr auto int_zero = []() { return 0; };

using IntLazy =
    LazySegmentTree<int, int_add, int_zero, int, int_add, int_add, int_zero>;

template <typename Pred>
concept CanMaxRight =
    requires(IntLazy& seg, Pred pred) { seg.max_right(0, pred); };

template <typename Pred>
concept CanMinLeft =
    requires(IntLazy& seg, Pred pred) { seg.min_left(0, pred); };

static_assert(CanInstantiateLazy<SumLen, sum_op, sum_e, Affine, aff_mapping_val,
                                 aff_composition, aff_id>);
static_assert(CanInstantiateLazy<SumLen, sum_op, sum_e, Affine, aff_mapping_ref,
                                 aff_composition, aff_id>);
static_assert(CanInstantiateLazy<NoDefaultS, nd_op, nd_e, NoDefaultF,
                                 nd_mapping, nd_composition, nd_id>);
static_assert(
    CanInstantiateLazy<
        bool, [](bool a, bool b) { return a && b; }, []() { return true; },
        bool, [](bool f, bool x) { return f || x; },
        [](bool f, bool g) { return f || g; }, []() { return false; }>);
static_assert(!CanInstantiateLazy<const int, int_add, int_zero, int, int_add,
                                  int_add, int_zero>);
static_assert(!CanInstantiateLazy<int, int_add, int_zero, const int, int_add,
                                  int_add, int_zero>);
static_assert(!CanInstantiateLazy<int, int_add, int_zero, NonCopyable,
                                  [](const NonCopyable&, int x) { return x; },
                                  [](const NonCopyable&, const NonCopyable&) {
                                    return NonCopyable{};
                                  },
                                  []() { return NonCopyable{}; }>);
static_assert(!CanInstantiateLazy<int, int_add, int_zero, int,
                                  NonConstCallable{}, int_add, int_zero>);
static_assert(!CanInstantiateLazy<int, int_add, int_zero, int, int_add,
                                  NonConstCallable{}, int_zero>);
static_assert(!CanInstantiateLazy<int, int_add, int_zero, int, bad_map_mut,
                                  int_add, int_zero>);
static_assert(!CanInstantiateLazy<int, int_add, int_zero, int, bad_map_void,
                                  int_add, int_zero>);
static_assert(!CanInstantiateLazy<int, int_add, int_zero, int, int_add, int_add,
                                  bad_id_void>);
static_assert(
    !CanInstantiateLazy<int, int_add, int_zero, int,
                        (int (*)(int, int)) nullptr, int_add, int_zero>);
static_assert(!CanInstantiateLazy<int, int_add, int_zero, int, int_add,
                                  (int (*)(int, int)) nullptr, int_zero>);
static_assert(!CanInstantiateLazy<int, int_add, int_zero, int, int_add, int_add,
                                  (int (*)()) nullptr>);

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

template <typename S, auto op, auto e, typename F, auto mapping,
          auto composition, auto id>
void verify_all_ranges(
    LazySegmentTree<S, op, e, F, mapping, composition, id>& seg,
    const std::vector<S>& a) {
  int n = (int)a.size();
  assert(seg.size() == n);

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
  for (int i = 0; i < n; ++i) assert(seg.get(i) == a[i]);
}

void test_empty_and_small_exhaustive(std::mt19937_64& rng) {
  {
    LazySegmentTree<SumLen, sum_op, sum_e, Affine, aff_mapping_val,
                    aff_composition, aff_id>
        empty_default;
    assert(empty_default.size() == 0);
    assert(empty_default.prod(0, 0) == sum_e());
    assert(empty_default.all_prod() == sum_e());
    empty_default.apply(0, 0, aff_id());
    assert(empty_default.max_right(0, [](const SumLen&) { return true; }) == 0);
    assert(empty_default.min_left(0, [](const SumLen&) { return true; }) == 0);

    LazySegmentTree<SumLen, sum_op, sum_e, Affine, aff_mapping_val,
                    aff_composition, aff_id>
        empty_n(0);
    assert(empty_n.size() == 0);
    assert(empty_n.prod(0, 0) == sum_e());
    assert(empty_n.all_prod() == sum_e());
  }

  std::uniform_int_distribution<int> val_dist(0, 998244352);
  for (int n = 0; n <= 33; ++n) {
    std::vector<SumLen> a(n);
    for (int i = 0; i < n; ++i) a[i] = {Mint(val_dist(rng)), 1};

    LazySegmentTree<SumLen, sum_op, sum_e, Affine, aff_mapping_val,
                    aff_composition, aff_id>
        seg(a);
    verify_all_ranges(seg, a);

    LazySegmentTree<SumLen, sum_op, sum_e, Affine, aff_mapping_ref,
                    aff_composition, aff_id>
        seg_n(n);
    std::vector<SumLen> cur(n, sum_e());
    verify_all_ranges(seg_n, cur);
    for (int i = 0; i < n; ++i) {
      cur[i] = a[i];
      seg_n.set(i, a[i]);
    }
    verify_all_ranges(seg_n, cur);

    for (int step = 0; step < 12 && n > 0; ++step) {
      int kind = (int)(rng() % 3);
      if (kind == 0) {
        int l = (int)(rng() % (unsigned int)(n + 1));
        int r = (int)(rng() % (unsigned int)(n + 1));
        if (l > r) std::swap(l, r);
        Affine f = {Mint(val_dist(rng)), Mint(val_dist(rng))};
        seg.apply(l, r, f);
        for (int i = l; i < r; ++i) a[i] = aff_mapping_ref(f, a[i]);
      } else if (kind == 1) {
        int p = (int)(rng() % (unsigned int)n);
        Affine f = {Mint(val_dist(rng)), Mint(val_dist(rng))};
        seg.apply(p, f);
        a[p] = aff_mapping_ref(f, a[p]);
      } else {
        int p = (int)(rng() % (unsigned int)n);
        SumLen nv = {Mint(val_dist(rng)), 1};
        seg.set(p, nv);
        a[p] = nv;
      }
      if (step % 4 == 0 || step + 1 == 12) verify_all_ranges(seg, a);
    }
  }
}

void test_binary_search_exhaustive(std::mt19937_64& rng) {
  constexpr auto max_op = [](long long a, long long b) {
    return std::max(a, b);
  };
  constexpr auto max_e = []() { return 0LL; };
  constexpr auto add_map = [](long long f, long long x) { return f + x; };
  constexpr auto add_comp = [](long long f, long long g) { return f + g; };
  constexpr auto add_id = []() { return 0LL; };

  std::uniform_int_distribution<long long> val_dist(0, 4);
  for (int n = 0; n <= 33; ++n) {
    std::vector<long long> a(n);
    for (int i = 0; i < n; ++i) a[i] = val_dist(rng);
    LazySegmentTree<long long, max_op, max_e, long long, add_map, add_comp,
                    add_id>
        seg(a);

    auto check_all = [&]() {
      long long max_v = 0;
      for (long long v : a) max_v = std::max(max_v, v);
      for (long long limit = 0; limit <= max_v + 1; ++limit) {
        auto pred = [limit](long long s) { return s <= limit; };
        for (int l = 0; l <= n; ++l) {
          int expected_r = l;
          while (expected_r < n && a[expected_r] <= limit) ++expected_r;
          assert(seg.max_right(l, pred) == expected_r);
        }
        for (int r = 0; r <= n; ++r) {
          int expected_l = r;
          while (expected_l > 0 && a[expected_l - 1] <= limit) --expected_l;
          assert(seg.min_left(r, pred) == expected_l);
        }
      }
    };

    check_all();
    for (int t = 0; t < 4 && n > 0; ++t) {
      int l = (int)(rng() % (unsigned int)(n + 1));
      int r = (int)(rng() % (unsigned int)(n + 1));
      if (l > r) std::swap(l, r);
      long long d = val_dist(rng);
      seg.apply(l, r, d);
      for (int i = l; i < r; ++i) a[i] += d;
      check_all();
    }
  }
}

void test_callable_variants() {
  std::vector<int> vi = {1, 2, 3, 4, 5, 6};
  constexpr auto wide_op = [](int a, int b) -> long long {
    return (long long)a + b;
  };
  constexpr auto wide_map = [](int f, int x) -> long long {
    return (long long)f * x;
  };
  constexpr auto wide_comp = [](int f, int g) -> long long {
    return (long long)f * g;
  };
  LazySegmentTree<int, wide_op, []() -> long long { return 0LL; }, int,
                  wide_map, wide_comp, []() -> long long { return 1LL; }>
      seg(vi);
  verify_all_ranges(seg, vi);

  seg.apply(1, 5, 2);
  vi = {1, 4, 6, 8, 10, 6};
  verify_all_ranges(seg, vi);

  assert(seg.max_right(0, pred_val) == 1);
  assert(seg.max_right(0, pred_ref) == 1);
  assert(seg.max_right(0, PredFunctor{11}) == 3);
  assert(seg.min_left(6, PredFunctor{16}) == 4);
}

void test_non_commutative_and_non_default_constructible() {
  std::vector<std::string> words = {"ab", "Cd", "eFg", "H", "ij", "KlMn", "op"};
  constexpr auto str_op = [](const std::string& a, const std::string& b) {
    return a + b;
  };
  constexpr auto str_e = []() { return std::string(); };
  constexpr auto toggle_map = [](bool f, std::string s) {
    if (!f) return s;
    for (char& ch : s) {
      unsigned char uch = (unsigned char)ch;
      if (std::islower(uch)) {
        ch = (char)std::toupper(uch);
      } else if (std::isupper(uch)) {
        ch = (char)std::tolower(uch);
      }
    }
    return s;
  };
  constexpr auto toggle_comp = [](bool f, bool g) { return f != g; };
  constexpr auto toggle_id = []() { return false; };

  LazySegmentTree<std::string, str_op, str_e, bool, toggle_map, toggle_comp,
                  toggle_id>
      seg_str(words);
  verify_all_ranges(seg_str, words);

  seg_str.apply(1, 5, true);
  for (int i = 1; i < 5; ++i) words[i] = toggle_map(true, words[i]);
  seg_str.apply(3, 7, true);
  for (int i = 3; i < 7; ++i) words[i] = toggle_map(true, words[i]);
  seg_str.apply(2, true);
  words[2] = toggle_map(true, words[2]);
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

  std::vector<NoDefaultS> nd;
  for (int i = 1; i <= 17; ++i) nd.emplace_back(i * 11 - 5, 1);
  LazySegmentTree<NoDefaultS, nd_op, nd_e, NoDefaultF, nd_mapping,
                  nd_composition, nd_id>
      seg_nd(nd);
  verify_all_ranges(seg_nd, nd);
  seg_nd.apply(2, 13, NoDefaultF(0x55));
  for (int i = 2; i < 13; ++i) nd[i] = nd_mapping(NoDefaultF(0x55), nd[i]);
  seg_nd.set(7, NoDefaultS(999, 1));
  nd[7] = NoDefaultS(999, 1);
  verify_all_ranges(seg_nd, nd);

  LazySegmentTree<NoDefaultS, nd_op, nd_e, NoDefaultF, nd_mapping,
                  nd_composition, nd_id>
      seg_nd_n(9);
  std::vector<NoDefaultS> nd_init(9, nd_e());
  verify_all_ranges(seg_nd_n, nd_init);
}

void test_bool_monoid() {
  std::vector<bool> b1 = {true, true, false, true, true, true, false, true};
  LazySegmentTree<bool, [](bool a, bool b) { return a && b; },
                  []() { return true; }, bool,
                  [](bool f, bool x) { return f || x; },
                  [](bool f, bool g) { return f || g; }, []() { return false; }>
      seg_and(b1);
  verify_all_ranges(seg_and, b1);
  assert(seg_and.max_right(0, [](bool x) { return x; }) == 2);
  assert(seg_and.min_left(6, [](bool x) { return x; }) == 3);

  seg_and.apply(2, 5, true);
  for (int i = 2; i < 5; ++i) b1[i] = true;
  verify_all_ranges(seg_and, b1);
  assert(seg_and.max_right(0, [](bool x) { return x; }) == 6);

  seg_and.set(3, false);
  b1[3] = false;
  verify_all_ranges(seg_and, b1);
}

void test_copy_and_move() {
  std::vector<SumLen> v = {{Mint(1), 1}, {Mint(2), 1}, {Mint(3), 1},
                           {Mint(4), 1}, {Mint(5), 1}, {Mint(6), 1}};
  using SegAff = LazySegmentTree<SumLen, sum_op, sum_e, Affine, aff_mapping_ref,
                                 aff_composition, aff_id>;

  SegAff original(v);
  original.apply(1, 5, {Mint(2), Mint(3)});

  SegAff copy_ctor(original);
  SegAff copy_assign;
  copy_assign = original;

  original.apply(0, 6, {Mint(5), Mint(1)});
  assert(copy_ctor.size() == 6);
  assert(copy_assign.size() == 6);
  assert(copy_ctor.prod(1, 5).sum == Mint(40));
  assert(copy_assign.prod(0, 3).sum == Mint(17));

  SegAff moved_ctor(std::move(copy_ctor));
  assert(moved_ctor.prod(1, 5).sum == Mint(40));
  copy_ctor = original;
  assert(copy_ctor.get(0).sum == Mint(6));

  SegAff moved_assign;
  moved_assign = std::move(copy_assign);
  assert(moved_assign.prod(2, 6).sum == Mint(39));
  copy_assign = SegAff();
  assert(copy_assign.size() == 0);
  assert(copy_assign.all_prod() == sum_e());
}

void test_random_large(std::mt19937_64& rng) {
  std::uniform_int_distribution<int> val_dist(0, 998244352);
  for (int n : {63, 64, 65, 255, 256, 257, 1023, 1024, 1025}) {
    std::vector<SumLen> a(n);
    for (int i = 0; i < n; ++i) a[i] = {Mint(val_dist(rng)), 1};
    LazySegmentTree<SumLen, sum_op, sum_e, Affine, aff_mapping_ref,
                    aff_composition, aff_id>
        seg(a);

    std::uniform_int_distribution<int> pos_dist(0, n - 1);
    std::uniform_int_distribution<int> bound_dist(0, n);
    for (int q = 0; q < 250; ++q) {
      int kind = (int)(rng() % 5);
      if (kind == 0) {
        int l = bound_dist(rng);
        int r = bound_dist(rng);
        if (l > r) std::swap(l, r);
        Affine f = {Mint(val_dist(rng)), Mint(val_dist(rng))};
        seg.apply(l, r, f);
        for (int i = l; i < r; ++i) a[i] = aff_mapping_ref(f, a[i]);
      } else if (kind == 1) {
        int p = pos_dist(rng);
        Affine f = {Mint(val_dist(rng)), Mint(val_dist(rng))};
        seg.apply(p, f);
        a[p] = aff_mapping_ref(f, a[p]);
      } else if (kind == 2) {
        int p = pos_dist(rng);
        SumLen nv = {Mint(val_dist(rng)), 1};
        seg.set(p, nv);
        a[p] = nv;
      } else if (kind == 3) {
        int p = pos_dist(rng);
        assert(seg.get(p) == a[p]);
      } else {
        int l = bound_dist(rng);
        int r = bound_dist(rng);
        if (l > r) std::swap(l, r);
        SumLen expected = sum_e();
        for (int i = l; i < r; ++i) expected = sum_op(expected, a[i]);
        assert(seg.prod(l, r) == expected);
      }
    }
    SumLen expected_all = sum_e();
    for (int i = 0; i < n; ++i) expected_all = sum_op(expected_all, a[i]);
    assert(seg.all_prod() == expected_all);
  }
}

void test_random_binary_search_medium(std::mt19937_64& rng) {
  constexpr auto max_op = [](long long a, long long b) {
    return std::max(a, b);
  };
  constexpr auto max_e = []() { return 0LL; };
  constexpr auto add_map = [](long long f, long long x) { return f + x; };
  constexpr auto add_comp = [](long long f, long long g) { return f + g; };
  constexpr auto add_id = []() { return 0LL; };

  std::uniform_int_distribution<long long> val_dist(0, 100);
  for (int n : {100, 511, 512, 513}) {
    std::vector<long long> a(n);
    for (int i = 0; i < n; ++i) a[i] = val_dist(rng);
    LazySegmentTree<long long, max_op, max_e, long long, add_map, add_comp,
                    add_id>
        seg(a);
    std::uniform_int_distribution<int> pos_dist(0, n - 1);
    std::uniform_int_distribution<int> bound_dist(0, n);
    long long max_possible = 100;
    for (int q = 0; q < 200; ++q) {
      int kind = (int)(rng() % 4);
      if (kind == 0) {
        int l = bound_dist(rng);
        int r = bound_dist(rng);
        if (l > r) std::swap(l, r);
        long long d = val_dist(rng);
        seg.apply(l, r, d);
        for (int i = l; i < r; ++i) a[i] += d;
        max_possible += d;
      } else if (kind == 1) {
        int p = pos_dist(rng);
        long long nv = val_dist(rng);
        seg.set(p, nv);
        a[p] = nv;
      } else if (kind == 2) {
        int l = bound_dist(rng);
        std::uniform_int_distribution<long long> lim_dist(0, max_possible);
        long long limit = lim_dist(rng);
        int r = l;
        while (r < n && a[r] <= limit) ++r;
        assert(seg.max_right(l, [limit](long long x) { return x <= limit; }) ==
               r);
      } else {
        int r = bound_dist(rng);
        std::uniform_int_distribution<long long> lim_dist(0, max_possible);
        long long limit = lim_dist(rng);
        int l = r;
        while (l > 0 && a[l - 1] <= limit) --l;
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
  test_copy_and_move();
  test_random_large(rng);
  test_random_binary_search_medium(rng);

  return 0;
}
