// Repository: Library3372
// File: tests/math/dynamic_modint_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/math/dynamic_modint_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include "library3372/math/dynamic_modint.hpp"

#include <cassert>
#include <climits>
#include <numeric>
#include <random>
#include <sstream>
#include <type_traits>
#include <utility>

namespace {

using library3372::DynamicModInt;

template <typename Mint, typename T> void check_integer_type() {
  static_assert(!std::is_convertible_v<T, Mint>);
  static_assert(!std::is_assignable_v<Mint&, T>);
  static_assert(std::is_constructible_v<Mint, T>);

  int m = Mint::mod();
  Mint a(5);
  T x = (T)7;

  assert((a + x) == (a + Mint(x)));
  assert((x + a) == (Mint(x) + a));
  assert((a - x) == (a - Mint(x)));
  assert((x - a) == (Mint(x) - a));
  assert((a * x) == (a * Mint(x)));
  assert((x * a) == (Mint(x) * a));
  if (Mint(x) != 0) {
    if (std::gcd(Mint(x).val(), m) == 1) assert((a / x) == (a / Mint(x)));
  }
  if (std::gcd(a.val(), m) == 1) assert((x / a) == (Mint(x) / a));

  Mint b = a;
  b += x;
  assert(b == a + x);
  b = a;
  b -= x;
  assert(b == a - x);
  b = a;
  b *= x;
  assert(b == a * x);
  if (std::gcd(Mint(x).val(), m) == 1) {
    b = a;
    b /= x;
    assert(b == a / x);
  }

  assert((Mint(x) == x) && (x == Mint(x)));
  assert(!(Mint(x) != x) && !(x != Mint(x)));
  assert((Mint(x) + 1 != x) == (m != 1));
  assert((x != Mint(x) + 1) == (m != 1));
}

template <typename Mint> void test_all_integer_types(int m) {
  Mint::set_mod(m);
  check_integer_type<Mint, bool>();
  check_integer_type<Mint, char>();
  check_integer_type<Mint, signed char>();
  check_integer_type<Mint, unsigned char>();
  check_integer_type<Mint, wchar_t>();
  check_integer_type<Mint, char8_t>();
  check_integer_type<Mint, char16_t>();
  check_integer_type<Mint, char32_t>();
  check_integer_type<Mint, short>();
  check_integer_type<Mint, unsigned short>();
  check_integer_type<Mint, int>();
  check_integer_type<Mint, unsigned int>();
  check_integer_type<Mint, long>();
  check_integer_type<Mint, unsigned long>();
  check_integer_type<Mint, long long>();
  check_integer_type<Mint, unsigned long long>();
  check_integer_type<Mint, __int128>();
  check_integer_type<Mint, unsigned __int128>();
}

template <typename Mint> void test_modulus_boundaries(int m) {
  Mint::set_mod(m);
  assert(Mint::mod() == m);

  Mint z;
  assert(z.val() == 0);
  assert(Mint::raw(0).val() == 0);
  assert(Mint::raw(m - 1).val() == m - 1);

  assert(Mint(0).val() == 0);
  assert(Mint(m).val() == 0);
  assert(Mint(-1).val() == m - 1);
  assert(Mint(-m).val() == 0);
  assert(Mint(-m - 1).val() == m - 1);

  long long int_min_rem = (long long)INT_MIN % m;
  if (int_min_rem < 0) int_min_rem += m;
  assert(Mint(INT_MIN).val() == int_min_rem);
  assert(Mint(INT_MAX).val() == INT_MAX % m);
  assert(Mint(UINT_MAX).val() == (int)(UINT_MAX % (unsigned int)m));

  long long llong_min_rem = LLONG_MIN % (long long)m;
  if (llong_min_rem < 0) llong_min_rem += m;
  assert(Mint(LLONG_MIN).val() == llong_min_rem);
  assert(Mint(LLONG_MAX).val() == (int)(LLONG_MAX % (long long)m));
  assert(Mint(ULLONG_MAX).val() == (int)(ULLONG_MAX % (unsigned long long)m));

  const __int128 i128_min = (__int128)((unsigned __int128)1 << 127);
  const __int128 i128_max = ~i128_min;
  const unsigned __int128 u128_max = ~(unsigned __int128)0;
  long long min_rem = (long long)(i128_min % (__int128)m);
  if (min_rem < 0) min_rem += m;
  assert(Mint(i128_min).val() == min_rem);
  assert(Mint(i128_max).val() == (int)(i128_max % (__int128)m));
  assert(Mint(u128_max).val() == (int)(u128_max % (unsigned __int128)m));

  Mint mx = Mint::raw(m - 1);
  assert((mx + mx).val() == (m == 1 ? 0 : m - 2));
  assert((Mint(0) - mx).val() == (m == 1 ? 0 : 1));
  assert((mx * mx).val() == (m == 1 ? 0 : 1));

  Mint cur = Mint::raw(m - 1);
  ++cur;
  assert(cur.val() == 0);
  --cur;
  assert(cur.val() == m - 1);
  assert((+mx).val() == m - 1);
  assert((-mx).val() == (m == 1 ? 0 : 1));
  assert(cur++ == m - 1 && cur == 0);
  assert(cur-- == 0 && cur == m - 1);

  assert(Mint(0).pow(0).val() == (m == 1 ? 0 : 1));
  assert(Mint(0).pow(1).val() == 0);
  assert(Mint(0).pow(100).val() == 0);
  if (m >= 2) {
    assert(Mint::raw(1).inv().val() == 1);
    assert(Mint::raw(m - 1).inv().val() == m - 1);
  } else {
    assert(Mint::raw(0).inv().val() == 0);
  }

  std::ostringstream oss;
  oss << Mint::raw(m - 1) << ' ' << Mint(0);
  std::ostringstream expected;
  expected << (m - 1) << " 0";
  assert(oss.str() == expected.str());

  std::istringstream iss(
      "0 1 -1 +42 -0 +0 000123 -000123 "
      "-9223372036854775808 9223372036854775807 "
      "18446744073709551615 +18446744073709551615");
  Mint in[12];
  for (int i = 0; i < 12; ++i) iss >> in[i];
  assert(in[0] == Mint(0));
  assert(in[1] == Mint(1));
  assert(in[2] == Mint(-1));
  assert(in[3] == Mint(42));
  assert(in[4] == Mint(0));
  assert(in[5] == Mint(0));
  assert(in[6] == Mint(123));
  assert(in[7] == Mint(-123));
  assert(in[8] == Mint(LLONG_MIN));
  assert(in[9] == Mint(LLONG_MAX));
  assert(in[10] == Mint(ULLONG_MAX));
  assert(in[11] == Mint(ULLONG_MAX));
}

template <typename Mint>
void test_random(int m, std::mt19937_64& rng, int iterations) {
  Mint::set_mod(m);
  std::uniform_int_distribution<long long> dist(-2LL * m, 2LL * m);
  std::uniform_int_distribution<long long> exp_dist(0, 60);

  auto norm = [m](long long v) -> int {
    long long r = v % m;
    if (r < 0) r += m;
    return (int)r;
  };

  for (int i = 0; i < iterations; ++i) {
    long long x = dist(rng);
    long long y = dist(rng);
    Mint mx(x);
    Mint my(y);

    assert(mx.val() == norm(x));
    assert(my.val() == norm(y));
    assert((mx + my).val() == norm(x + y));
    assert((mx - my).val() == norm(x - y));
    assert((mx * my).val() == norm((long long)mx.val() * my.val()));
    assert((mx + y).val() == norm(x + y));
    assert((x + my).val() == norm(x + y));
    assert((mx - y).val() == norm(x - y));
    assert((x - my).val() == norm(x - y));
    assert((mx * y).val() == norm((long long)mx.val() * norm(y)));
    assert((x * my).val() == norm((long long)norm(x) * my.val()));
    assert((+mx).val() == mx.val());
    assert((-mx).val() == norm(-(long long)mx.val()));

    if (std::gcd(my.val(), m) == 1) {
      Mint inv_y = my.inv();
      assert((my * inv_y).val() == (m == 1 ? 0 : 1));
      assert((mx / my) == mx * inv_y);
      assert((mx / y) == mx * inv_y);
    }

    long long e = exp_dist(rng);
    long long p = 1 % m;
    for (long long k = 0; k < e; ++k) p = (p * mx.val()) % m;
    assert(mx.pow(e).val() == (int)p);
  }
}

void test_multiple_ids_and_reset() {
  using MintA = DynamicModInt<0>;
  using MintB = DynamicModInt<1>;
  using MintNeg = DynamicModInt<-1>;

  assert(MintA::mod() == 998244353);
  assert(MintB::mod() == 998244353);
  assert(MintNeg::mod() == 998244353);

  MintA::set_mod(1000000007);
  MintB::set_mod(998244353);
  MintNeg::set_mod(17);

  assert(MintA::mod() == 1000000007);
  assert(MintB::mod() == 998244353);
  assert(MintNeg::mod() == 17);

  MintA a1(1000000000), a2(10);
  MintB b1(998244350), b2(10);
  MintNeg n1(15), n2(5);

  assert((a1 + a2).val() == 3);
  assert((b1 + b2).val() == 7);
  assert((n1 * n2).val() == 7);

  MintA::set_mod(13);
  assert(MintA::mod() == 13);
  assert(MintB::mod() == 998244353);
  assert(MintNeg::mod() == 17);
  assert((MintA(10) * MintA(10)).val() == 9);
}

void test_type_constraints() {
  using Mint = DynamicModInt<0>;

  static_assert(sizeof(DynamicModInt<-1>) == sizeof(unsigned int));
  static_assert(sizeof(DynamicModInt<0>) == sizeof(unsigned int));
  static_assert(sizeof(DynamicModInt<100>) == sizeof(unsigned int));
  static_assert(std::is_same_v<decltype(Mint::mod()), int>);
  static_assert(std::is_same_v<decltype(Mint::set_mod(1)), void>);
  static_assert(std::is_same_v<decltype(std::declval<Mint>().val()), int>);

  static_assert(std::is_constructible_v<Mint, bool>);
  static_assert(std::is_constructible_v<Mint, char>);
  static_assert(!std::is_constructible_v<Mint, double>);
  static_assert(!std::is_convertible_v<int, Mint>);
  static_assert(!std::is_convertible_v<long long, Mint>);
  static_assert(!std::is_assignable_v<Mint&, int>);
  static_assert(!std::is_assignable_v<Mint&, long long>);
}

}  // namespace

int main() {
  test_type_constraints();
  test_multiple_ids_and_reset();

  using Mint = DynamicModInt<42>;
  test_all_integer_types<Mint>(1);
  test_all_integer_types<Mint>(12);
  test_all_integer_types<Mint>(998244353);
  test_all_integer_types<Mint>(INT_MAX);

  test_modulus_boundaries<Mint>(1);
  test_modulus_boundaries<Mint>(2);
  test_modulus_boundaries<Mint>(12);
  test_modulus_boundaries<Mint>(998244353);
  test_modulus_boundaries<Mint>(1000000007);
  test_modulus_boundaries<Mint>(INT_MAX);

  std::mt19937_64 rng(0x5f3759df12345678ULL);
  test_random<Mint>(1, rng, 200);
  test_random<Mint>(2, rng, 200);
  test_random<Mint>(12, rng, 500);
  test_random<Mint>(998244353, rng, 1000);
  test_random<Mint>(1000000007, rng, 1000);
  test_random<Mint>(INT_MAX, rng, 1000);

  return 0;
}
