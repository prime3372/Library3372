// Repository: Library3372
// File: tests/data_structure/range_add_range_sum_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/data_structure/range_add_range_sum_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include "library3372/data_structure/range_add_range_sum.hpp"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>
#include <random>
#include <type_traits>
#include <utility>
#include <vector>

#include "library3372/math/dynamic_modint.hpp"
#include "library3372/math/static_modint.hpp"

namespace {

using library3372::DynamicModInt;
using library3372::RangeAddRangeSum;
using library3372::StaticModInt;

struct MissingSub {
  MissingSub& operator+=(const MissingSub&) { return *this; }
};

struct NoDefaultGroup {
  explicit NoDefaultGroup(int) {}
  NoDefaultGroup& operator+=(const NoDefaultGroup&) { return *this; }
  NoDefaultGroup& operator-=(const NoDefaultGroup&) { return *this; }
};

struct MissingMul {
  MissingMul& operator+=(const MissingMul&) { return *this; }
  MissingMul& operator-=(const MissingMul&) { return *this; }
};

struct OnlyBinaryMul {
  OnlyBinaryMul& operator+=(const OnlyBinaryMul&) { return *this; }
  OnlyBinaryMul& operator-=(const OnlyBinaryMul&) { return *this; }
  friend OnlyBinaryMul operator*(int, const OnlyBinaryMul& v) { return v; }
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

  Vec2& operator*=(int k) {
    x *= k;
    y *= k;
    return *this;
  }

  friend bool operator==(const Vec2&, const Vec2&) = default;
};

template <typename T>
concept CanInstantiate = requires { typename RangeAddRangeSum<T>; };

static_assert(CanInstantiate<int>);
static_assert(CanInstantiate<long long>);
static_assert(CanInstantiate<float>);
static_assert(CanInstantiate<double>);
static_assert(CanInstantiate<__float128>);
static_assert(CanInstantiate<Vec2>);
static_assert(!CanInstantiate<bool>);
static_assert(!CanInstantiate<const int>);
static_assert(!CanInstantiate<volatile int>);
static_assert(!CanInstantiate<int&>);
static_assert(!CanInstantiate<MissingSub>);
static_assert(!CanInstantiate<MissingMul>);
static_assert(!CanInstantiate<OnlyBinaryMul>);

template <typename T>
concept HasBinarySearch = requires(const RangeAddRangeSum<T>& fw, const T& x) {
  fw.max_right(0, x);
  fw.min_left(0, x);
};

static_assert(HasBinarySearch<int>);
static_assert(HasBinarySearch<long long>);
static_assert(HasBinarySearch<__int128>);
static_assert(!HasBinarySearch<double>);
static_assert(!HasBinarySearch<StaticModInt<998244353>>);
static_assert(!HasBinarySearch<Vec2>);
static_assert(!CanInstantiate<NoDefaultGroup>);

template <typename T> T wrap_add(T a, T b) {
  if constexpr (std::is_same_v<T, __int128>) {
    return (T)((unsigned __int128)a + (unsigned __int128)b);
  } else if constexpr (std::is_integral_v<T> && std::is_signed_v<T>) {
    using U = std::make_unsigned_t<T>;
    return (T)(U)((U)a + (U)b);
  } else {
    a += b;
    return a;
  }
}

template <typename T>
void verify_state(const RangeAddRangeSum<T>& fw, const std::vector<T>& a) {
  int n = (int)a.size();
  assert(fw.size() == n);
  for (int i = 0; i < n; ++i) assert(fw.get(i) == a[i]);
  for (int l = 0; l <= n; ++l) {
    T acc{};
    for (int r = l; r <= n; ++r) {
      assert(fw.sum(l, r) == acc);
      if (r < n) acc = wrap_add(acc, a[r]);
    }
  }
}

template <typename T> void test_integer_type(std::mt19937_64& rng) {
  {
    RangeAddRangeSum<T> empty_default;
    assert(empty_default.size() == 0);
    assert(empty_default.sum(0, 0) == T{});
    empty_default.add(0, 0, (T)5);

    RangeAddRangeSum<T> empty_n(0);
    assert(empty_n.size() == 0);
    assert(empty_n.sum(0, 0) == T{});
  }

  for (int n : {0, 1, 2, 3, 4, 7, 8, 9, 15, 16, 17, 32, 33}) {
    std::vector<T> a(n);
    for (int i = 0; i < n; ++i) a[i] = (T)rng();
    RangeAddRangeSum<T> fw(a);
    verify_state(fw, a);

    RangeAddRangeSum<T> fw_n(n);
    std::vector<T> zeros(n, T{});
    verify_state(fw_n, zeros);

    for (int step = 0; step < 20 && n > 0; ++step) {
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
      if (step % 5 == 0 || step + 1 == 20) verify_state(fw, a);
    }
  }
}

void test_wraparound_ub_free() {
  int n = 10;
  RangeAddRangeSum<int> fw(n);
  int big = std::numeric_limits<int>::max() - 3;
  fw.add(0, 10, big);
  fw.add(2, 7, big);
  unsigned int expected_2 = (unsigned int)big + (unsigned int)big;
  assert(fw.get(2) == (int)expected_2);
  unsigned int expected_sum =
      5U * (unsigned int)big + 5U * (unsigned int)expected_2;
  assert(fw.sum(0, 10) == (int)expected_sum);
}

void test_modint_and_custom_ring(std::mt19937_64& rng) {
  using Mint = StaticModInt<998244353>;
  {
    int n = 25;
    std::vector<Mint> a(n);
    for (int i = 0; i < n; ++i) a[i] = Mint(rng());
    RangeAddRangeSum<Mint> fw(a);
    verify_state(fw, a);
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

  using DMint = DynamicModInt<1>;
  DMint::set_mod(1000000007);
  {
    int n = 20;
    std::vector<DMint> a(n);
    for (int i = 0; i < n; ++i) a[i] = DMint(rng());
    RangeAddRangeSum<DMint> fw(a);
    fw.add(3, 15, DMint(123456789));
    for (int i = 3; i < 15; ++i) a[i] += DMint(123456789);
    fw.set(5, DMint(42));
    a[5] = DMint(42);
    verify_state(fw, a);
  }
  {
    int n = 15;
    std::vector<Vec2> a(n);
    for (int i = 0; i < n; ++i) a[i] = {i + 1, -(i + 2)};
    RangeAddRangeSum<Vec2> fw(a);
    fw.add(2, 11, Vec2{3, -5});
    for (int i = 2; i < 11; ++i) a[i] += Vec2{3, -5};
    fw.set(4, Vec2{100, 200});
    a[4] = Vec2{100, 200};
    verify_state(fw, a);
  }
}

template <typename T> void test_float_type() {
  std::vector<T> a = {(T)1.5, (T)0.25, (T)2.0, (T)0.125, (T)4.5};
  RangeAddRangeSum<T> fw(a);
  fw.add(1, 4, (T)1.25);
  assert(std::fabs((double)(fw.get(0) - (T)1.5)) < 1e-6);
  assert(std::fabs((double)(fw.get(2) - (T)3.25)) < 1e-6);
  assert(std::fabs((double)(fw.sum(0, 5) - (T)12.125)) < 1e-6);
  fw.set(2, (T)-0.5);
  assert(std::fabs((double)(fw.sum(1, 4) - (T)2.375)) < 1e-6);
}

void test_random_large(std::mt19937_64& rng) {
  for (int n : {100, 511, 512, 513}) {
    std::vector<long long> a(n);
    for (int i = 0; i < n; ++i) {
      a[i] = (long long)(rng() % 2000001ULL) - 1000000LL;
    }
    RangeAddRangeSum<long long> fw(a);
    std::uniform_int_distribution<int> pos_dist(0, n - 1);
    std::uniform_int_distribution<int> bound_dist(0, n);
    for (int q = 0; q < 300; ++q) {
      int kind = (int)(rng() % 5);
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
      } else if (kind == 3) {
        int p = pos_dist(rng);
        assert(fw.get(p) == a[p]);
      } else {
        int l = bound_dist(rng);
        int r = bound_dist(rng);
        if (l > r) std::swap(l, r);
        long long expected = 0;
        for (int i = l; i < r; ++i) expected += a[i];
        assert(fw.sum(l, r) == expected);
      }
    }
  }
}

void test_binary_search_edges() {
  {
    RangeAddRangeSum<long long> fw0;
    assert(fw0.max_right(0, 0) == 0);
    assert(fw0.min_left(0, 0) == 0);
  }
  {
    std::vector<long long> b = {-100, -50, 0, 3, 0, 5, 0, 2, 0};
    RangeAddRangeSum<long long> fw(b);
    for (int l = 2; l <= (int)b.size(); ++l) {
      for (long long lim : {0LL, 1LL, 2LL, 3LL, 4LL, 8LL, 10LL, 100LL}) {
        long long s = 0;
        int exp_r = l;
        while (exp_r < (int)b.size() && s + b[exp_r] <= lim) {
          s += b[exp_r++];
        }
        assert(fw.max_right(l, lim) == exp_r);
      }
    }
    assert(fw.max_right(2, 1000000000LL) == (int)b.size());
    assert(fw.max_right(3, 0LL) == 3);
    assert(fw.max_right(2, 0LL) == 3);
  }
  {
    std::vector<long long> b = {0, 2, 0, 5, 0, 3, 0, -50, -100};
    RangeAddRangeSum<long long> fw(b);
    for (int r = 0; r <= 7; ++r) {
      for (long long lim : {0LL, 1LL, 2LL, 3LL, 5LL, 7LL, 10LL, 100LL}) {
        long long s = 0;
        int exp_l = r;
        while (exp_l > 0 && s + b[exp_l - 1] <= lim) {
          s += b[--exp_l];
        }
        assert(fw.min_left(r, lim) == exp_l);
      }
    }
    assert(fw.min_left(7, 1000000000LL) == 0);
    assert(fw.min_left(6, 0LL) == 6);
    assert(fw.min_left(7, 0LL) == 6);
  }
  {
    RangeAddRangeSum<signed char> fw(
        std::vector<signed char>{-100, -100, 0, 10, 20, 30});
    assert(fw.max_right(2, (signed char)29) == 4);
    assert(fw.max_right(2, (signed char)30) == 5);
    assert(fw.max_right(2, (signed char)60) == 6);
  }
}

template <typename T>
void test_random_binary_search(std::mt19937_64& rng, int n, int ops) {
  std::uniform_int_distribution<long long> val_dist(0, 10);
  std::vector<T> init(n);
  for (int i = 0; i < n; ++i) init[i] = (T)val_dist(rng);

  RangeAddRangeSum<T> fw(init);
  std::vector<T> a = init;
  std::uniform_int_distribution<int> pos_dist(0, n - 1);
  std::uniform_int_distribution<int> bound_dist(0, n);

  for (int step = 0; step < ops; ++step) {
    int kind = (int)(rng() % 4);
    if (kind == 0) {
      int l = bound_dist(rng);
      int r = bound_dist(rng);
      if (l > r) std::swap(l, r);
      T x = (T)val_dist(rng);
      fw.add(l, r, x);
      for (int i = l; i < r; ++i) a[i] += x;
    } else if (kind == 1) {
      int p = pos_dist(rng);
      T x = (T)val_dist(rng);
      fw.set(p, x);
      a[p] = x;
    } else if (kind == 2) {
      int l = bound_dist(rng);
      long long rem = 0;
      for (int i = l; i < n; ++i) rem += (long long)a[i];
      long long max_lim = rem + 10;
      if constexpr (std::is_same_v<T, signed char>) {
        if (rem + 10 > 127) continue;
      }
      std::uniform_int_distribution<long long> lim_dist(0, max_lim);
      T lim = (T)lim_dist(rng);
      int exp_r = l;
      long long acc = 0;
      while (exp_r < n && acc + (long long)a[exp_r] <= (long long)lim) {
        acc += (long long)a[exp_r++];
      }
      assert(fw.max_right(l, lim) == exp_r);
    } else {
      int r = bound_dist(rng);
      long long pref = 0;
      for (int i = 0; i < r; ++i) pref += (long long)a[i];
      if constexpr (std::is_same_v<T, signed char>) {
        if (pref + 10 > 127) continue;
      }
      std::uniform_int_distribution<long long> lim_dist(0, pref + 10);
      T lim = (T)lim_dist(rng);
      int exp_l = r;
      long long acc = 0;
      while (exp_l > 0 && acc + (long long)a[exp_l - 1] <= (long long)lim) {
        acc += (long long)a[--exp_l];
      }
      assert(fw.min_left(r, lim) == exp_l);
    }
  }
}

}  // namespace

int main() {
  std::mt19937_64 rng(0x9e3779b97f4a7c15ULL);

  test_integer_type<signed char>(rng);
  test_integer_type<unsigned char>(rng);
  test_integer_type<short>(rng);
  test_integer_type<unsigned short>(rng);
  test_integer_type<int>(rng);
  test_integer_type<unsigned int>(rng);
  test_integer_type<long long>(rng);
  test_integer_type<unsigned long long>(rng);
  test_integer_type<__int128>(rng);
  test_integer_type<unsigned __int128>(rng);
  test_wraparound_ub_free();
  test_binary_search_edges();
  test_float_type<float>();
  test_float_type<double>();
  test_float_type<long double>();
  test_float_type<__float128>();
  test_modint_and_custom_ring(rng);
  test_random_large(rng);

  for (int n : {1, 2, 3, 7, 8, 9, 15, 16, 17, 32, 64, 128, 257}) {
    test_random_binary_search<int>(rng, n, std::min(n * 3, 300));
    test_random_binary_search<unsigned int>(rng, n, std::min(n * 3, 300));
    test_random_binary_search<long long>(rng, n, std::min(n * 3, 300));
    test_random_binary_search<unsigned long long>(rng, n, std::min(n * 3, 300));
    if (n <= 10) test_random_binary_search<signed char>(rng, n, 20);
    if (n <= 64) test_random_binary_search<__int128>(rng, n, 120);
  }

  return 0;
}
