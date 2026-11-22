// Repository: Library3372
// File: tests/data_structure/fenwick_tree_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/data_structure/fenwick_tree_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include "library3372/data_structure/fenwick_tree.hpp"

#include <cassert>
#include <climits>
#include <cmath>
#include <random>
#include <type_traits>
#include <vector>

#include "library3372/math/static_modint.hpp"

namespace {

using library3372::FenwickTree;
using library3372::StaticModInt;

template <typename T> class NaiveFenwickTree {
 public:
  explicit NaiveFenwickTree(int n) : a_(n, T{}) {}

  template <typename U>
  explicit NaiveFenwickTree(const std::vector<U>& a) : a_(a.size()) {
    for (int i = 0; i < (int)a.size(); ++i) a_[i] = (T)a[i];
  }

  int size() const { return (int)a_.size(); }

  void add(int p, const T& x) { a_[p] += x; }

  void set(int p, const T& x) { a_[p] = x; }

  T get(int p) const { return a_[p]; }

  T sum(int r) const { return sum(0, r); }

  T sum(int l, int r) const {
    T res{};
    for (int i = l; i < r; ++i) res += a_[i];
    return res;
  }

  template <typename F> int max_right(int l, F pred) const {
    T s{};
    int r = l;
    while (r < (int)a_.size()) {
      T next_s = s;
      next_s += a_[r];
      if (!pred(next_s)) break;
      s = next_s;
      ++r;
    }
    return r;
  }

  template <typename F> int min_left(int r, F pred) const {
    T s{};
    int l = r;
    while (l > 0) {
      T next_s = s;
      next_s += a_[l - 1];
      if (!pred(next_s)) break;
      s = next_s;
      --l;
    }
    return l;
  }

 private:
  std::vector<T> a_;
};

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
void assert_all_match(const FenwickTree<T>& ft,
                      const NaiveFenwickTree<T>& naive) {
  int n = naive.size();
  assert(ft.size() == n);
  for (int i = 0; i < n; ++i) {
    assert(ft.get(i) == naive.get(i));
    assert(ft.sum(i) == naive.sum(i));
  }
  assert(ft.sum(n) == naive.sum(n));
  for (int l = 0; l <= n; ++l) {
    for (int r = l; r <= n; ++r) {
      assert(ft.sum(l, r) == naive.sum(l, r));
    }
  }
}

void test_empty_and_single() {
  FenwickTree<long long> ft_default;
  assert(ft_default.size() == 0);
  assert(ft_default.sum(0) == 0);
  assert(ft_default.sum(0, 0) == 0);
  assert(ft_default.max_right([](long long s) { return s <= 0; }) == 0);
  assert(ft_default.max_right(0, [](long long s) { return s <= 0; }) == 0);
  assert(ft_default.min_left(0, [](long long s) { return s <= 0; }) == 0);

  FenwickTree<long long> ft0(0);
  assert(ft0.size() == 0);
  assert(ft0.sum(0) == 0);
  assert(ft0.sum(0, 0) == 0);
  assert(ft0.max_right([](long long s) { return s <= 0; }) == 0);
  assert(ft0.min_left(0, [](long long s) { return s <= 0; }) == 0);

  std::vector<long long> empty_vec;
  FenwickTree<long long> ft_vec0(empty_vec);
  assert(ft_vec0.size() == 0);
  assert(ft_vec0.sum(0, 0) == 0);

  FenwickTree<long long> ft1(1);
  assert(ft1.size() == 1);
  assert(ft1.get(0) == 0);
  assert(ft1.sum(0) == 0);
  assert(ft1.sum(1) == 0);
  ft1.add(0, 5);
  assert(ft1.get(0) == 5);
  assert(ft1.sum(0, 1) == 5);
  assert(ft1.max_right([](long long s) { return s < 5; }) == 0);
  assert(ft1.max_right([](long long s) { return s <= 5; }) == 1);
  assert(ft1.min_left(1, [](long long s) { return s < 5; }) == 1);
  assert(ft1.min_left(1, [](long long s) { return s <= 5; }) == 0);
  ft1.set(0, -3);
  assert(ft1.get(0) == -3);
  assert(ft1.sum(1) == -3);
}

void test_constructors_and_ctad() {
  std::vector<int> vi = {1, 2, 3, 4, 5};
  FenwickTree ft_ctad(vi);
  static_assert(std::is_same_v<decltype(ft_ctad), FenwickTree<int>>);
  assert(ft_ctad.size() == 5);
  assert(ft_ctad.sum(5) == 15);

  FenwickTree<long long> ft_init({10, 20, 30, 40});
  assert(ft_init.size() == 4);
  assert(ft_init.sum(1, 3) == 50);

  FenwickTree<long long> ft_conv(vi);
  assert(ft_conv.size() == 5);
  assert(ft_conv.sum(5) == 15);

  using Mint = StaticModInt<998244353>;
  std::vector<long long> vll = {-1, 998244353LL + 7, 1000000000000LL};
  FenwickTree<Mint> ft_mint(vll);
  assert(ft_mint.size() == 3);
  assert(ft_mint.get(0) == Mint(-1));
  assert(ft_mint.get(1) == Mint(7));
  assert(ft_mint.get(2) == Mint(1000000000000LL));
  assert(ft_mint.sum(3) == Mint(-1) + Mint(7) + Mint(1000000000000LL));
}

void test_signed_wrap_safety() {
  std::vector<int> init = {1500000000, 500000000, -2000000000, 2000000000};
  FenwickTree<int> ft(init);
  assert(ft.get(0) == 1500000000);
  assert(ft.get(1) == 500000000);
  assert(ft.get(2) == -2000000000);
  assert(ft.get(3) == 2000000000);
  assert(ft.sum(0) == 0);
  assert(ft.sum(1) == 1500000000);
  assert(ft.sum(2) == 2000000000);
  assert(ft.sum(3) == 0);
  assert(ft.sum(4) == 2000000000);
  assert(ft.sum(1, 3) == -1500000000);
  assert(ft.sum(2, 4) == 0);

  ft.set(2, 2000000000);
  assert(ft.get(2) == 2000000000);
  ft.set(0, -1500000000);
  assert(ft.get(0) == -1500000000);
  assert(ft.sum(1, 3) == -1794967296);
  // Check exact sum when prefix sums remain within int range.
  ft.set(1, -500000000);
  ft.set(2, 0);
  ft.set(3, 1000000000);
  assert(ft.sum(4) == -1000000000);
  assert(ft.sum(1, 4) == 500000000);
}

void test_binary_search_edges() {
  // Test negative elements before l in max_right(l, pred).
  {
    std::vector<long long> b = {-100, -50, 0, 3, 0, 5, 0, 2, 0};
    FenwickTree<long long> ftb(b);
    NaiveFenwickTree<long long> nb(b);
    for (int l = 2; l <= (int)b.size(); ++l) {
      for (long long lim : {0LL, 1LL, 2LL, 3LL, 4LL, 8LL, 10LL, 100LL}) {
        auto pred = [&](long long s) { return s <= lim; };
        assert(ftb.max_right(l, pred) == nb.max_right(l, pred));
      }
    }
    assert(ftb.max_right(2, [](long long) { return true; }) == (int)b.size());
    assert(ftb.max_right(3, [](long long s) { return s == 0; }) == 3);
    assert(ftb.max_right(2, [](long long s) { return s == 0; }) == 3);
  }

  // Test negative elements after r in min_left(r, pred).
  {
    std::vector<long long> b = {0, 2, 0, 5, 0, 3, 0, -50, -100};
    FenwickTree<long long> ftb(b);
    NaiveFenwickTree<long long> nb(b);
    for (int r = 0; r <= 7; ++r) {
      for (long long lim : {0LL, 1LL, 2LL, 3LL, 5LL, 7LL, 10LL, 100LL}) {
        auto pred = [&](long long s) { return s <= lim; };
        assert(ftb.min_left(r, pred) == nb.min_left(r, pred));
      }
    }
    assert(ftb.min_left(7, [](long long) { return true; }) == 0);
    assert(ftb.min_left(6, [](long long s) { return s == 0; }) == 6);
    assert(ftb.min_left(7, [](long long s) { return s == 0; }) == 6);
  }
}

template <typename T> void test_integer_type(std::mt19937_64& rng) {
  std::uniform_int_distribution<int> val_dist(-2, 2);
  for (int n : {0, 1, 2, 7, 8, 15, 16, 17, 32}) {
    std::vector<T> init(n);
    for (int i = 0; i < n; ++i) init[i] = (T)val_dist(rng);

    FenwickTree<T> ft(init);
    NaiveFenwickTree<T> naive(init);
    assert_all_match(ft, naive);

    if (n == 0) continue;
    std::uniform_int_distribution<int> pos_dist(0, n - 1);
    for (int step = 0; step < 30; ++step) {
      int p = pos_dist(rng);
      T x = (T)val_dist(rng);
      if (step % 2 == 0) {
        ft.add(p, x);
        naive.add(p, x);
      } else {
        ft.set(p, x);
        naive.set(p, x);
      }
    }
    assert_all_match(ft, naive);
  }
}

template <typename T> void test_float_type() {
  std::vector<T> init = {(T)1.5, (T)0.25, (T)2.0, (T)0.125, (T)4.5};
  FenwickTree<T> ft(init);
  assert(std::fabs((double)(ft.get(2) - (T)2.0)) < 1e-6);
  assert(std::fabs((double)(ft.sum(5) - (T)8.375)) < 1e-6);
  assert(std::fabs((double)(ft.sum(1, 4) - (T)2.375)) < 1e-6);
  ft.add(1, (T)0.75);
  ft.set(4, (T)1.5);
  assert(std::fabs((double)(ft.sum(5) - (T)6.125)) < 1e-6);
  int r = ft.max_right([](const T& s) { return s <= (T)3.5; });
  assert(r == 2);
  int l = ft.min_left(5, [](const T& s) { return s <= (T)2.0; });
  assert(l == 3);
}

void test_modint_and_custom_struct(std::mt19937_64& rng) {
  {
    using Mint = StaticModInt<998244353>;
    int n = 25;
    std::uniform_int_distribution<int> vdist(0, 998244352);
    std::vector<Mint> init(n);
    for (int i = 0; i < n; ++i) init[i] = Mint(vdist(rng));
    FenwickTree<Mint> ft(init);
    NaiveFenwickTree<Mint> naive(init);
    for (int step = 0; step < 50; ++step) {
      int p = (int)(rng() % (unsigned)n);
      Mint x(vdist(rng));
      if (step % 2 == 0) {
        ft.add(p, x);
        naive.add(p, x);
      } else {
        ft.set(p, x);
        naive.set(p, x);
      }
    }
    assert_all_match(ft, naive);
  }
  {
    using Mint1 = StaticModInt<1>;
    FenwickTree<Mint1> ft(5);
    ft.add(2, Mint1(10));
    assert(ft.sum(5) == Mint1(0));
  }
  {
    std::vector<Vec2> init = {{1, -2}, {3, 4}, {-5, 6}, {0, 1}, {2, -3}};
    FenwickTree<Vec2> ft(init);
    NaiveFenwickTree<Vec2> naive(init);
    assert_all_match(ft, naive);
    ft.add(2, Vec2{10, -4});
    naive.add(2, Vec2{10, -4});
    ft.set(0, Vec2{-7, 8});
    naive.set(0, Vec2{-7, 8});
    assert_all_match(ft, naive);
  }
}

void test_random_comprehensive(std::mt19937_64& rng, int n, int ops) {
  std::uniform_int_distribution<long long> init_dist(-1000, 1000);
  std::vector<long long> init(n);
  for (int i = 0; i < n; ++i) init[i] = init_dist(rng);

  FenwickTree<long long> ft =
      (n % 2 == 0) ? FenwickTree<long long>(init) : FenwickTree<long long>(n);
  NaiveFenwickTree<long long> naive = (n % 2 == 0)
                                          ? NaiveFenwickTree<long long>(init)
                                          : NaiveFenwickTree<long long>(n);

  std::uniform_int_distribution<int> pos_dist(0, n - 1);
  std::uniform_int_distribution<int> bound_dist(0, n);
  std::uniform_int_distribution<int> op_dist(0, 4);

  for (int step = 0; step < ops; ++step) {
    int op = op_dist(rng);
    if (op == 0) {
      int p = pos_dist(rng);
      long long x = init_dist(rng);
      ft.add(p, x);
      naive.add(p, x);
    } else if (op == 1) {
      int p = pos_dist(rng);
      long long x = init_dist(rng);
      ft.set(p, x);
      naive.set(p, x);
    } else if (op == 2) {
      int p = pos_dist(rng);
      assert(ft.get(p) == naive.get(p));
    } else if (op == 3) {
      int r = bound_dist(rng);
      assert(ft.sum(r) == naive.sum(r));
    } else {
      int l = bound_dist(rng);
      int r = bound_dist(rng);
      if (l > r) std::swap(l, r);
      assert(ft.sum(l, r) == naive.sum(l, r));
    }
  }
  assert_all_match(ft, naive);
}

void test_random_binary_search(std::mt19937_64& rng, int n, int ops) {
  std::uniform_int_distribution<long long> val_dist(0, 20);
  std::vector<long long> init(n);
  for (int i = 0; i < n; ++i) init[i] = val_dist(rng);

  FenwickTree<long long> ft(init);
  NaiveFenwickTree<long long> naive(init);

  std::uniform_int_distribution<int> pos_dist(0, n - 1);
  std::uniform_int_distribution<int> bound_dist(0, n);
  std::uniform_int_distribution<int> op_dist(0, 4);

  for (int step = 0; step < ops; ++step) {
    int op = op_dist(rng);
    if (op == 0) {
      int p = pos_dist(rng);
      long long x = val_dist(rng);
      ft.add(p, x);
      naive.add(p, x);
    } else if (op == 1) {
      int p = pos_dist(rng);
      long long x = val_dist(rng);
      ft.set(p, x);
      naive.set(p, x);
    } else if (op == 2) {
      long long total = naive.sum(n);
      std::uniform_int_distribution<long long> lim_dist(0, total + 10);
      long long lim = lim_dist(rng);
      auto pred = [&](long long s) { return s <= lim; };
      assert(ft.max_right(pred) == naive.max_right(0, pred));
    } else if (op == 3) {
      int l = bound_dist(rng);
      long long rem = naive.sum(l, n);
      std::uniform_int_distribution<long long> lim_dist(0, rem + 10);
      long long lim = lim_dist(rng);
      auto pred = [&](long long s) { return s <= lim; };
      assert(ft.max_right(l, pred) == naive.max_right(l, pred));
    } else {
      int r = bound_dist(rng);
      long long pref = naive.sum(r);
      std::uniform_int_distribution<long long> lim_dist(0, pref + 10);
      long long lim = lim_dist(rng);
      auto pred = [&](long long s) { return s <= lim; };
      assert(ft.min_left(r, pred) == naive.min_left(r, pred));
    }
  }
}

}  // namespace

int main() {
  test_empty_and_single();
  test_constructors_and_ctad();
  test_signed_wrap_safety();
  test_binary_search_edges();

  std::mt19937_64 rng(0x517cc1b727220a95ULL);

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

  for (int n : {1, 2, 3, 4, 7, 8, 15, 16, 17, 32, 64, 127, 128}) {
    for (int trial = 0; trial < 3; ++trial) {
      test_random_comprehensive(rng, n, n * 3);
      test_random_binary_search(rng, n, n * 3);
    }
  }
  return 0;
}
