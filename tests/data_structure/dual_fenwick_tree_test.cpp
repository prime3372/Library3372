// Repository: Library3372
// File: tests/data_structure/dual_fenwick_tree_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/data_structure/dual_fenwick_tree_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include "library3372/data_structure/dual_fenwick_tree.hpp"

#include <cassert>
#include <cmath>
#include <limits>
#include <random>
#include <type_traits>
#include <utility>
#include <vector>

#include "library3372/math/dynamic_modint.hpp"
#include "library3372/math/static_modint.hpp"

namespace {

using library3372::DualFenwickTree;
using library3372::DynamicModInt;
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

struct MissingSub {
  MissingSub& operator+=(const MissingSub&) { return *this; }
};

struct NoDefaultGroup {
  explicit NoDefaultGroup(int) {}
  NoDefaultGroup& operator+=(const NoDefaultGroup&) { return *this; }
  NoDefaultGroup& operator-=(const NoDefaultGroup&) { return *this; }
};

template <typename T>
concept CanInstantiate = requires { typename DualFenwickTree<T>; };

static_assert(CanInstantiate<int>);
static_assert(CanInstantiate<Vec2>);
static_assert(!CanInstantiate<bool>);
static_assert(!CanInstantiate<const int>);
static_assert(!CanInstantiate<volatile int>);
static_assert(!CanInstantiate<int&>);
static_assert(!CanInstantiate<MissingSub>);
static_assert(!CanInstantiate<NoDefaultGroup>);

template <typename T> T wrap_add(T a, T b) {
  if constexpr (std::is_same_v<T, __int128>) {
    return (T)((unsigned __int128)a + (unsigned __int128)b);
  } else if constexpr (std::is_integral_v<T> && std::is_signed_v<T>) {
    using U = std::make_unsigned_t<T>;
    return (T)(U)((U)a + (U)b);
  } else {
    return (T)(a + b);
  }
}

template <typename T>
void verify_state(const DualFenwickTree<T>& fw, const std::vector<T>& a) {
  int n = (int)a.size();
  assert(fw.size() == n);
  for (int i = 0; i < n; ++i) assert(fw.get(i) == a[i]);
}

template <typename T> void test_integer_type(std::mt19937_64& rng) {
  {
    DualFenwickTree<T> empty_default;
    assert(empty_default.size() == 0);
    empty_default.add(0, 0, (T)5);

    DualFenwickTree<T> empty_n(0);
    assert(empty_n.size() == 0);
  }

  for (int n : {0, 1, 2, 3, 4, 7, 8, 9, 15, 16, 17, 32, 33}) {
    std::vector<T> a(n);
    for (int i = 0; i < n; ++i) a[i] = (T)rng();
    DualFenwickTree<T> fw(a);
    verify_state(fw, a);

    DualFenwickTree<T> fw_n(n);
    std::vector<T> zeros(n, T{});
    verify_state(fw_n, zeros);

    for (int step = 0; step < 25 && n > 0; ++step) {
      int kind = (int)(rng() % 3);
      if (kind == 0) {
        int l = (int)(rng() % (unsigned int)(n + 1));
        int r = (int)(rng() % (unsigned int)(n + 1));
        if (l > r) std::swap(l, r);
        T x = (T)rng();
        fw.add(l, r, x);
        for (int i = l; i < r; ++i) a[i] = wrap_add(a[i], x);
      } else if (kind == 1) {
        int p = (int)(rng() % (unsigned int)n);
        T x = (T)rng();
        fw.add(p, x);
        a[p] = wrap_add(a[p], x);
      } else {
        int p = (int)(rng() % (unsigned int)n);
        T x = (T)rng();
        fw.set(p, x);
        a[p] = x;
      }
      if (step % 5 == 0 || step + 1 == 25) verify_state(fw, a);
    }
  }
}

void test_exhaustive_ranges() {
  for (int n = 1; n <= 17; ++n) {
    DualFenwickTree<long long> fw(n);
    std::vector<long long> a(n, 0);
    long long v = 1;
    for (int l = 0; l <= n; ++l) {
      for (int r = l; r <= n; ++r) {
        fw.add(l, r, v);
        for (int i = l; i < r; ++i) a[i] += v;
        ++v;
      }
    }
    verify_state(fw, a);
  }
}

template <typename T> void test_float_type() {
  std::vector<T> a = {(T)1.5, (T)0.25, (T)2.0, (T)0.125, (T)4.5};
  DualFenwickTree<T> fw(a);
  fw.add(1, 4, (T)1.25);
  assert(std::fabs((double)(fw.get(0) - (T)1.5)) < 1e-6);
  assert(std::fabs((double)(fw.get(1) - (T)1.5)) < 1e-6);
  assert(std::fabs((double)(fw.get(2) - (T)3.25)) < 1e-6);
  assert(std::fabs((double)(fw.get(4) - (T)4.5)) < 1e-6);
  fw.set(2, (T)-0.5);
  assert(std::fabs((double)(fw.get(2) - (T)-0.5)) < 1e-6);
}

void test_modint_and_custom_struct(std::mt19937_64& rng) {
  using Mint = StaticModInt<998244353>;
  {
    int n = 25;
    std::vector<Mint> a(n);
    for (int i = 0; i < n; ++i) a[i] = Mint(rng());
    DualFenwickTree<Mint> fw(a);
    for (int step = 0; step < 30; ++step) {
      int l = (int)(rng() % (unsigned int)(n + 1));
      int r = (int)(rng() % (unsigned int)(n + 1));
      if (l > r) std::swap(l, r);
      Mint x(rng());
      fw.add(l, r, x);
      for (int i = l; i < r; ++i) a[i] += x;
    }
    verify_state(fw, a);
  }

  using DMint = DynamicModInt<2>;
  DMint::set_mod(1000000007);
  {
    int n = 20;
    std::vector<DMint> a(n);
    for (int i = 0; i < n; ++i) a[i] = DMint(rng());
    DualFenwickTree<DMint> fw(a);
    fw.add(3, 18, DMint(987654321));
    for (int i = 3; i < 18; ++i) a[i] += DMint(987654321);
    fw.set(10, DMint(7));
    a[10] = DMint(7);
    verify_state(fw, a);
  }

  {
    int n = 20;
    std::vector<Vec2> a(n);
    for (int i = 0; i < n; ++i) {
      a[i] = Vec2{(long long)(rng() % 1000), (long long)(rng() % 1000)};
    }
    DualFenwickTree<Vec2> fw(a);
    Vec2 d{123, -456};
    fw.add(2, 17, d);
    for (int i = 2; i < 17; ++i) a[i] += d;
    verify_state(fw, a);
  }
}

void test_random_large(std::mt19937_64& rng) {
  for (int n : {100, 511, 512, 513}) {
    std::vector<long long> a(n);
    for (int i = 0; i < n; ++i) {
      a[i] = (long long)(rng() % 2000001ULL) - 1000000LL;
    }
    DualFenwickTree<long long> fw(a);
    std::uniform_int_distribution<int> pos_dist(0, n - 1);
    std::uniform_int_distribution<int> bound_dist(0, n);
    for (int q = 0; q < 300; ++q) {
      int kind = (int)(rng() % 4);
      if (kind < 2) {
        int l = bound_dist(rng);
        int r = bound_dist(rng);
        if (l > r) std::swap(l, r);
        long long x = (long long)(rng() % 2000001ULL) - 1000000LL;
        fw.add(l, r, x);
        for (int i = l; i < r; ++i) a[i] += x;
      } else if (kind == 2) {
        int p = pos_dist(rng);
        long long x = (long long)(rng() % 2000001ULL) - 1000000LL;
        fw.set(p, x);
        a[p] = x;
      } else {
        int p = pos_dist(rng);
        assert(fw.get(p) == a[p]);
      }
    }
  }
}

}  // namespace

int main() {
  std::mt19937_64 rng(0x9e3779b97f4a7c15ULL);

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
  test_exhaustive_ranges();
  test_float_type<float>();
  test_float_type<double>();
  test_float_type<long double>();
  test_float_type<__float128>();
  test_modint_and_custom_struct(rng);
  test_random_large(rng);

  return 0;
}
