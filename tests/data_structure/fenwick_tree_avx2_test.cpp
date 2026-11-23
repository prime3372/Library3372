// Repository: Library3372
// File: tests/data_structure/fenwick_tree_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/data_structure/fenwick_tree_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef __AVX2__
#error "AVX2 must be enabled to test the AVX2 implementation of FenwickTree."
#endif

#include "library3372/data_structure/fenwick_tree.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <random>
#include <utility>
#include <vector>

#include "library3372/math/static_modint.hpp"
#include "tests/data_structure/fenwick_tree_test_common.hpp"

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

}  // namespace

int main() {
  assert(__builtin_cpu_supports("avx2"));

  std::mt19937_64 rng(0x517cc1b727220a95ULL);
  run_common_fenwick_tree_tests(rng);

  test_signed_wrap_safety_other_widths();

  test_integer_type<signed char>(rng);
  test_integer_type<unsigned char>(rng);
  test_integer_type<char>(rng);
  test_integer_type<short>(rng);
  test_integer_type<unsigned short>(rng);
  test_integer_type<__int128>(rng);
  test_integer_type<unsigned __int128>(rng);

  test_float_type<float>();
  test_float_type<double>();
  test_float_type<long double>();
  test_float_type<__float128>();

  test_modint_and_custom_struct(rng);

  for (int n : {1, 2, 3, 4, 7, 8, 15, 16, 17, 32, 64, 127, 128}) {
    for (int trial = 0; trial < 3; ++trial) {
      test_random_binary_search<signed char>(rng, std::min(n, 5), 20);
      test_random_binary_search<short>(rng, n, std::min(n * 3, 200));
      test_random_binary_search<__int128>(rng, n, std::min(n * 3, 200));
    }
  }
  return 0;
}
