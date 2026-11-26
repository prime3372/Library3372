// Repository: Library3372
// File: tests/data_structure/fenwick_tree_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/data_structure/fenwick_tree_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include "library3372/data_structure/fenwick_tree.hpp"

#include <algorithm>
#include <cassert>
#include <climits>
#include <cmath>
#include <random>
#include <type_traits>
#include <utility>
#include <vector>

#include "library3372/math/static_modint.hpp"

namespace {

using library3372::FenwickTree;
using library3372::StaticModInt;

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

template <typename T>
concept HasBinarySearch = requires(const FenwickTree<T>& fw, const T& x) {
  fw.max_right(0, x);
  fw.min_left(0, x);
};

static_assert(HasBinarySearch<int>);
static_assert(HasBinarySearch<long long>);
static_assert(HasBinarySearch<__int128>);
static_assert(!HasBinarySearch<double>);
static_assert(!HasBinarySearch<StaticModInt<998244353>>);
static_assert(!HasBinarySearch<Vec2>);

struct MissingSub {
  MissingSub& operator+=(const MissingSub&) { return *this; }
};

struct NoDefaultGroup {
  explicit NoDefaultGroup(int) {}
  NoDefaultGroup& operator+=(const NoDefaultGroup&) { return *this; }
  NoDefaultGroup& operator-=(const NoDefaultGroup&) { return *this; }
};

template <typename T>
concept CanInstantiateFw = requires { typename FenwickTree<T>; };

static_assert(CanInstantiateFw<int>);
static_assert(CanInstantiateFw<Vec2>);
static_assert(!CanInstantiateFw<bool>);
static_assert(!CanInstantiateFw<const int>);
static_assert(!CanInstantiateFw<int&>);
static_assert(!CanInstantiateFw<MissingSub>);
static_assert(!CanInstantiateFw<NoDefaultGroup>);

template <typename T> class NaiveFenwickTree {
 public:
  explicit NaiveFenwickTree(int n) : a_(n, T{}) {}

  explicit NaiveFenwickTree(const std::vector<T>& a) : a_(a) {}

  void add(int p, const T& x) { a_[p] += x; }

  void set(int p, const T& x) { a_[p] = x; }

  T sum(int r) const { return sum(0, r); }

  T sum(int l, int r) const {
    T res{};
    for (int i = l; i < r; ++i) res += a_[i];
    return res;
  }

  int max_right(int l, const T& x) const {
    T s{};
    int r = l;
    while (r < (int)a_.size()) {
      T next_s = s;
      next_s += a_[r];
      if (!(next_s <= x)) break;
      s = next_s;
      ++r;
    }
    return r;
  }

  int min_left(int r, const T& x) const {
    T s{};
    int l = r;
    while (l > 0) {
      T next_s = s;
      next_s += a_[l - 1];
      if (!(next_s <= x)) break;
      s = next_s;
      --l;
    }
    return l;
  }

  int size() const { return (int)a_.size(); }

 private:
  std::vector<T> a_;
};

template <typename T>
void assert_all_match(const FenwickTree<T>& fw,
                      const NaiveFenwickTree<T>& naive) {
  int n = naive.size();
  assert(fw.size() == n);
  for (int i = 0; i < n; ++i) assert(fw.get(i) == naive.sum(i, i + 1));
  for (int l = 0; l <= n; ++l) {
    for (int r = l; r <= n; ++r) {
      assert(fw.sum(l, r) == naive.sum(l, r));
    }
  }
}

void test_empty_and_single() {
  FenwickTree<long long> fw_default;
  assert(fw_default.size() == 0);
  assert(fw_default.sum(0, 0) == 0);
  assert(fw_default.max_right(0, 0) == 0);
  assert(fw_default.min_left(0, 0) == 0);

  FenwickTree<long long> fw0(0);
  assert(fw0.size() == 0);
  assert(fw0.sum(0, 0) == 0);
  assert(fw0.max_right(0, 0) == 0);
  assert(fw0.min_left(0, 0) == 0);

  std::vector<long long> empty_vec;
  FenwickTree<long long> fw_vec0(empty_vec);
  assert(fw_vec0.size() == 0);
  assert(fw_vec0.sum(0, 0) == 0);

  FenwickTree<long long> fw1(1);
  assert(fw1.size() == 1);
  assert(fw1.sum(0, 0) == 0);
  assert(fw1.sum(0, 1) == 0);
  fw1.add(0, 5);
  assert(fw1.sum(0, 1) == 5);
  assert(fw1.max_right(0, 0) == 0);
  assert(fw1.max_right(0, 4) == 0);
  assert(fw1.max_right(0, 5) == 1);
  assert(fw1.min_left(1, 0) == 1);
  assert(fw1.min_left(1, 4) == 1);
  assert(fw1.min_left(1, 5) == 0);
  fw1.set(0, -3);
  assert(fw1.sum(0, 1) == -3);
}

void test_constructors_and_ctad_integers() {
  std::vector<int> vi = {1, 2, 3, 4, 5};
  FenwickTree fw_ctad(vi);
  static_assert(std::is_same_v<decltype(fw_ctad), FenwickTree<int>>);
  assert(fw_ctad.size() == 5);
  assert(fw_ctad.sum(0, 5) == 15);

  FenwickTree<long long> fw_init({10, 20, 30, 40});
  assert(fw_init.size() == 4);
  assert(fw_init.sum(1, 3) == 50);
}

void test_signed_wrap_safety_32_64() {
  std::vector<int> init = {1500000000, 500000000, -2000000000, 2000000000};
  FenwickTree<int> fw(init);
  assert(fw.sum(0, 0) == 0);
  assert(fw.sum(0, 1) == 1500000000);
  assert(fw.sum(0, 2) == 2000000000);
  assert(fw.sum(0, 3) == 0);
  assert(fw.sum(0, 4) == 2000000000);
  assert(fw.sum(1, 3) == -1500000000);
  assert(fw.sum(2, 4) == 0);

  fw.set(2, 2000000000);
  assert(fw.sum(2, 3) == 2000000000);
  fw.set(0, -1500000000);
  assert(fw.sum(0, 1) == -1500000000);
  assert(fw.sum(1, 3) == -1794967296);
  fw.set(1, -500000000);
  fw.set(2, 0);
  fw.set(3, 1000000000);
  assert(fw.sum(0, 4) == -1000000000);
  assert(fw.sum(1, 4) == 500000000);
  assert(fw.get(0) == -1500000000);
  assert(fw.get(3) == 1000000000);

  std::vector<long long> a = {LLONG_MAX, LLONG_MAX, LLONG_MIN, 1,        2,
                              3,         4,         5,         LLONG_MAX};
  FenwickTree<long long> fw64(a);
  unsigned long long u0 =
      (unsigned long long)LLONG_MAX + (unsigned long long)LLONG_MAX;
  assert(fw64.sum(0, 2) == (long long)u0);
  unsigned long long u1 = u0 + (unsigned long long)LLONG_MIN + 15ULL +
                          (unsigned long long)LLONG_MAX;
  assert(fw64.sum(0, 9) == (long long)u1);
  fw64.add(0, LLONG_MAX);
  fw64.set(8, LLONG_MIN);
  unsigned long long u2 =
      u1 + (unsigned long long)LLONG_MAX +
      ((unsigned long long)LLONG_MIN - (unsigned long long)LLONG_MAX);
  assert(fw64.sum(0, 9) == (long long)u2);
}

void test_signed_wrap_safety_other_widths() {
  {
    std::vector<signed char> a = {100, 100, -100, -100, 80, 80};
    FenwickTree<signed char> fw8(a);
    assert(fw8.sum(0, 2) == (signed char)(unsigned char)200);
    assert(fw8.sum(0, 4) == 0);
    assert(fw8.sum(4, 6) == (signed char)(unsigned char)160);
    fw8.add(0, (signed char)100);
    assert(fw8.sum(0, 2) == (signed char)(unsigned char)300);
  }
  {
    std::vector<short> a = {30000, 30000, -30000, -30000};
    FenwickTree<short> fw16(a);
    assert(fw16.sum(0, 2) == (short)(unsigned short)60000);
    assert(fw16.sum(0, 4) == 0);
    fw16.set(3, (short)30000);
    assert(fw16.sum(0, 4) == (short)(unsigned short)60000);
  }
  {
    __int128 half = (__int128)1 << 126;
    std::vector<__int128> a = {half, half, half, -half};
    FenwickTree<__int128> fw128(a);
    unsigned __int128 uhalf = (unsigned __int128)half;
    assert(fw128.sum(0, 2) == (__int128)(uhalf * 2));
    assert(fw128.sum(0, 4) == (__int128)(uhalf * 2));
    fw128.add(1, half);
    assert(fw128.sum(0, 4) == (__int128)(uhalf * 3));
  }
}

void test_binary_search_edges() {
  {
    std::vector<long long> b = {-100, -50, 0, 3, 0, 5, 0, 2, 0};
    FenwickTree<long long> fwb(b);
    NaiveFenwickTree<long long> nb(b);
    for (int l = 2; l <= (int)b.size(); ++l) {
      for (long long lim : {0LL, 1LL, 2LL, 3LL, 4LL, 8LL, 10LL, 100LL}) {
        assert(fwb.max_right(l, lim) == nb.max_right(l, lim));
      }
    }
    assert(fwb.max_right(2, 1000000000LL) == (int)b.size());
    assert(fwb.max_right(3, 0LL) == 3);
    assert(fwb.max_right(2, 0LL) == 3);
  }
  {
    std::vector<int> b = {2000000000, 0, 1, 2, 0, 3};
    FenwickTree<int> fwb(b);
    NaiveFenwickTree<int> nb(b);
    for (int l = 1; l <= (int)b.size(); ++l) {
      for (int lim : {0, 1, 2, 3, 6, 100}) {
        assert(fwb.max_right(l, lim) == nb.max_right(l, lim));
      }
    }
  }
  {
    std::vector<long long> b = {0, 2, 0, 5, 0, 3, 0, -50, -100};
    FenwickTree<long long> fwb(b);
    NaiveFenwickTree<long long> nb(b);
    for (int r = 0; r <= 7; ++r) {
      for (long long lim : {0LL, 1LL, 2LL, 3LL, 5LL, 7LL, 10LL, 100LL}) {
        assert(fwb.min_left(r, lim) == nb.min_left(r, lim));
      }
    }
    assert(fwb.min_left(7, 1000000000LL) == 0);
    assert(fwb.min_left(6, 0LL) == 6);
    assert(fwb.min_left(7, 0LL) == 6);
  }
}

template <typename T> void test_integer_type(std::mt19937_64& rng) {
  std::uniform_int_distribution<int> val_dist(-2, 2);
  for (int n : {0, 1, 2, 7, 8, 15, 16, 17, 32}) {
    std::vector<T> init(n);
    for (int i = 0; i < n; ++i) init[i] = (T)val_dist(rng);

    FenwickTree<T> fw(init);
    NaiveFenwickTree<T> naive(init);
    assert_all_match(fw, naive);

    if (n == 0) continue;
    std::uniform_int_distribution<int> pos_dist(0, n - 1);
    for (int step = 0; step < 30; ++step) {
      int p = pos_dist(rng);
      T x = (T)val_dist(rng);
      if (step % 2 == 0) {
        fw.add(p, x);
        naive.add(p, x);
      } else {
        fw.set(p, x);
        naive.set(p, x);
      }
    }
    assert_all_match(fw, naive);
  }
}

template <typename T> void test_float_type() {
  std::vector<T> init = {(T)1.5, (T)0.25, (T)2.0, (T)0.125, (T)4.5};
  FenwickTree<T> fw(init);
  assert(std::fabs((double)(fw.sum(2, 3) - (T)2.0)) < 1e-6);
  assert(std::fabs((double)(fw.sum(0, 5) - (T)8.375)) < 1e-6);
  assert(std::fabs((double)(fw.sum(1, 4) - (T)2.375)) < 1e-6);
  fw.add(1, (T)0.75);
  fw.set(4, (T)1.5);
  assert(std::fabs((double)(fw.sum(0, 5) - (T)6.125)) < 1e-6);
}

void test_modint_and_custom_struct(std::mt19937_64& rng) {
  {
    using Mint = StaticModInt<998244353>;
    std::vector<Mint> vm = {-1, 998244353LL + 7, 1000000000000LL};
    FenwickTree<Mint> fw_mint(vm);
    assert(fw_mint.size() == 3);
    assert(fw_mint.sum(0, 1) == -1);
    assert(fw_mint.sum(1, 2) == 7);
    assert(fw_mint.sum(0, 3) == Mint(-1) + 7 + 1000000000000LL);

    int n = 25;
    std::uniform_int_distribution<int> vdist(0, 998244352);
    std::vector<Mint> init(n);
    for (int i = 0; i < n; ++i) init[i] = vdist(rng);
    FenwickTree<Mint> fw(init);
    NaiveFenwickTree<Mint> naive(init);
    for (int step = 0; step < 50; ++step) {
      int p = (int)(rng() % (unsigned)n);
      Mint x = vdist(rng);
      if (step % 2 == 0) {
        fw.add(p, x);
        naive.add(p, x);
      } else {
        fw.set(p, x);
        naive.set(p, x);
      }
    }
    assert_all_match(fw, naive);
  }
  {
    using Mint1 = StaticModInt<1>;
    FenwickTree<Mint1> fw(5);
    fw.add(2, 10);
    assert(fw.sum(0, 5) == 0);
  }
  {
    FenwickTree<Vec2> fw0(0);
    assert(fw0.size() == 0);
    assert(fw0.sum(0, 0) == (Vec2{0, 0}));

    std::uniform_int_distribution<long long> val_dist(-1000, 1000);
    for (int n : {1, 8, 25, 64}) {
      std::vector<Vec2> init(n);
      for (int i = 0; i < n; ++i) {
        init[i] = Vec2{val_dist(rng), val_dist(rng)};
      }
      FenwickTree<Vec2> fw =
          (n % 2 == 0) ? FenwickTree<Vec2>(init) : FenwickTree<Vec2>(n);
      NaiveFenwickTree<Vec2> naive = (n % 2 == 0) ? NaiveFenwickTree<Vec2>(init)
                                                  : NaiveFenwickTree<Vec2>(n);
      std::uniform_int_distribution<int> pos_dist(0, n - 1);
      std::uniform_int_distribution<int> bound_dist(0, n);
      for (int step = 0; step < n * 4; ++step) {
        int op = (int)(rng() % 3);
        if (op == 0) {
          int p = pos_dist(rng);
          Vec2 v{val_dist(rng), val_dist(rng)};
          fw.add(p, v);
          naive.add(p, v);
        } else if (op == 1) {
          int p = pos_dist(rng);
          Vec2 v{val_dist(rng), val_dist(rng)};
          fw.set(p, v);
          naive.set(p, v);
        } else {
          int l = bound_dist(rng);
          int r = bound_dist(rng);
          if (l > r) std::swap(l, r);
          assert(fw.sum(l, r) == naive.sum(l, r));
        }
      }
      assert_all_match(fw, naive);
    }
  }
}

void test_random_comprehensive(std::mt19937_64& rng, int n, int ops) {
  std::uniform_int_distribution<long long> init_dist(-1000, 1000);
  std::vector<long long> init(n);
  for (int i = 0; i < n; ++i) init[i] = init_dist(rng);

  FenwickTree<long long> fw =
      (n % 2 == 0) ? FenwickTree<long long>(init) : FenwickTree<long long>(n);
  NaiveFenwickTree<long long> naive = (n % 2 == 0)
                                          ? NaiveFenwickTree<long long>(init)
                                          : NaiveFenwickTree<long long>(n);

  std::uniform_int_distribution<int> pos_dist(0, n - 1);
  std::uniform_int_distribution<int> bound_dist(0, n);
  std::uniform_int_distribution<int> op_dist(0, 2);

  for (int step = 0; step < ops; ++step) {
    int op = op_dist(rng);
    if (op == 0) {
      int p = pos_dist(rng);
      long long x = init_dist(rng);
      fw.add(p, x);
      naive.add(p, x);
    } else if (op == 1) {
      int p = pos_dist(rng);
      long long x = init_dist(rng);
      fw.set(p, x);
      naive.set(p, x);
    } else {
      int l = bound_dist(rng);
      int r = bound_dist(rng);
      if (l > r) std::swap(l, r);
      assert(fw.sum(l, r) == naive.sum(l, r));
    }
  }
  assert_all_match(fw, naive);
}

template <typename T>
void test_random_binary_search(std::mt19937_64& rng, int n, int ops) {
  std::uniform_int_distribution<long long> val_dist(0, 20);
  std::vector<T> init(n);
  for (int i = 0; i < n; ++i) init[i] = (T)val_dist(rng);

  FenwickTree<T> fw(init);
  NaiveFenwickTree<T> naive(init);

  std::uniform_int_distribution<int> pos_dist(0, n - 1);
  std::uniform_int_distribution<int> bound_dist(0, n);
  std::uniform_int_distribution<int> op_dist(0, 3);

  for (int step = 0; step < ops; ++step) {
    int op = op_dist(rng);
    if (op == 0) {
      int p = pos_dist(rng);
      T x = (T)val_dist(rng);
      fw.add(p, x);
      naive.add(p, x);
    } else if (op == 1) {
      int p = pos_dist(rng);
      T x = (T)val_dist(rng);
      fw.set(p, x);
      naive.set(p, x);
    } else if (op == 2) {
      int l = bound_dist(rng);
      T rem = naive.sum(l, n);
      std::uniform_int_distribution<long long> lim_dist(0, (long long)rem + 10);
      T lim = (T)lim_dist(rng);
      assert(fw.max_right(l, lim) == naive.max_right(l, lim));
    } else {
      int r = bound_dist(rng);
      T pref = naive.sum(r);
      std::uniform_int_distribution<long long> lim_dist(0,
                                                        (long long)pref + 10);
      T lim = (T)lim_dist(rng);
      assert(fw.min_left(r, lim) == naive.min_left(r, lim));
    }
  }
}

}  // namespace

int main() {
  std::mt19937_64 rng(0x517cc1b727220a95ULL);

  test_empty_and_single();
  test_constructors_and_ctad_integers();
  test_signed_wrap_safety_32_64();
  test_signed_wrap_safety_other_widths();
  test_binary_search_edges();

  test_integer_type<signed char>(rng);
  test_integer_type<unsigned char>(rng);
  test_integer_type<char>(rng);
  test_integer_type<short>(rng);
  test_integer_type<unsigned short>(rng);
  test_integer_type<int>(rng);
  test_integer_type<unsigned int>(rng);
  test_integer_type<long>(rng);
  test_integer_type<unsigned long>(rng);
  test_integer_type<long long>(rng);
  test_integer_type<unsigned long long>(rng);
  test_integer_type<__int128>(rng);
  test_integer_type<unsigned __int128>(rng);

  test_float_type<float>();
  test_float_type<double>();
  test_float_type<long double>();
  test_float_type<__float128>();

  test_modint_and_custom_struct(rng);

  for (int n :
       {1, 2, 3, 4, 7, 8, 15, 16, 17, 32, 64, 127, 128, 511, 512, 513}) {
    int trials = (n <= 128) ? 3 : 1;
    for (int trial = 0; trial < trials; ++trial) {
      if (n <= 128) test_random_comprehensive(rng, n, n * 3);
      test_random_binary_search<int>(rng, n, std::min(n * 3, 400));
      test_random_binary_search<unsigned int>(rng, n, std::min(n * 3, 400));
      test_random_binary_search<long long>(rng, n, std::min(n * 3, 400));
      test_random_binary_search<unsigned long long>(rng, n,
                                                    std::min(n * 3, 400));
    }
  }

  for (int n : {1, 2, 3, 4, 7, 8, 15, 16, 17, 32, 64, 127, 128}) {
    for (int trial = 0; trial < 3; ++trial) {
      test_random_binary_search<signed char>(rng, std::min(n, 5), 20);
      test_random_binary_search<short>(rng, n, std::min(n * 3, 200));
      test_random_binary_search<__int128>(rng, n, std::min(n * 3, 200));
    }
  }

  return 0;
}
