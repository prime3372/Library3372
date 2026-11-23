// Repository: Library3372
// File: tests/math/static_modint_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/math/static_modint_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include "library3372/math/static_modint.hpp"

#include <cassert>
#include <climits>
#include <numeric>
#include <random>
#include <sstream>
#include <type_traits>
#include <utility>

namespace {

using library3372::StaticModInt;

template <int m, typename T> void check_integer_type() {
  using Mint = StaticModInt<m>;

  static_assert(std::is_convertible_v<T, Mint>);
  static_assert(std::is_assignable_v<Mint&, T>);
  static_assert(std::is_constructible_v<Mint, T>);

  Mint a = 5;
  T x = (T)7;
  Mint c = x;
  assert(c == Mint(x));
  c = (T)3;
  assert(c == Mint((T)3));

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

template <int m> void test_all_integer_types() {
  check_integer_type<m, bool>();
  check_integer_type<m, char>();
  check_integer_type<m, signed char>();
  check_integer_type<m, unsigned char>();
  check_integer_type<m, wchar_t>();
  check_integer_type<m, char8_t>();
  check_integer_type<m, char16_t>();
  check_integer_type<m, char32_t>();
  check_integer_type<m, short>();
  check_integer_type<m, unsigned short>();
  check_integer_type<m, int>();
  check_integer_type<m, unsigned int>();
  check_integer_type<m, long>();
  check_integer_type<m, unsigned long>();
  check_integer_type<m, long long>();
  check_integer_type<m, unsigned long long>();
  check_integer_type<m, __int128>();
  check_integer_type<m, unsigned __int128>();
}

constexpr bool test_constexpr() {
  using Mint = StaticModInt<998244353>;
  static_assert(Mint::mod() == 998244353);
  Mint a;
  if (a.val() != 0) return false;
  Mint b = 123;
  Mint c = Mint::raw(456);
  Mint d = b + c;
  if (d.val() != 579) return false;
  d = d - 1000;
  if (d.val() != 998244353 - 421) return false;
  Mint e = 2 * Mint(3) + 4;
  if (e != 10) return false;
  if (e.pow(3) != 1000) return false;
  if ((e / 2) * 2 != e) return false;
  if (e.inv() * e != 1) return false;
  if (+e != e) return false;
  if (-e != 998244353 - 10) return false;
  ++e;
  if (e != 11) return false;
  if (e++ != 11 || e != 12) return false;
  --e;
  if (e != 11) return false;
  if (e-- != 11 || e != 10) return false;
  return true;
}

template <int m> void test_modulus_boundaries() {
  using Mint = StaticModInt<m>;
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

template <int m> void test_random(std::mt19937_64& rng, int iterations) {
  using Mint = StaticModInt<m>;
  std::uniform_int_distribution<long long> dist(-2LL * m, 2LL * m);
  std::uniform_int_distribution<long long> exp_dist(0, 60);

  auto norm = [](long long v) -> int {
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

void test_type_constraints() {
  using Mint = StaticModInt<998244353>;

  static_assert(sizeof(StaticModInt<1>) == sizeof(unsigned int));
  static_assert(sizeof(StaticModInt<998244353>) == sizeof(unsigned int));
  static_assert(sizeof(StaticModInt<INT_MAX>) == sizeof(unsigned int));
  static_assert(std::is_same_v<decltype(Mint::mod()), int>);
  static_assert(std::is_same_v<decltype(std::declval<Mint>().val()), int>);

  static_assert(std::is_constructible_v<Mint, bool>);
  static_assert(std::is_constructible_v<Mint, char>);
  static_assert(!std::is_constructible_v<Mint, double>);
  static_assert(!std::is_convertible_v<double, Mint>);
  static_assert(!std::is_assignable_v<Mint&, double>);
  static_assert(std::is_convertible_v<int, Mint>);
  static_assert(std::is_convertible_v<long long, Mint>);
  static_assert(std::is_assignable_v<Mint&, int>);
  static_assert(std::is_assignable_v<Mint&, long long>);
  static_assert(test_constexpr());
}

}  // namespace

int main() {
  test_type_constraints();

  test_all_integer_types<1>();
  test_all_integer_types<12>();
  test_all_integer_types<998244353>();
  test_all_integer_types<INT_MAX>();

  test_modulus_boundaries<1>();
  test_modulus_boundaries<2>();
  test_modulus_boundaries<12>();
  test_modulus_boundaries<998244353>();
  test_modulus_boundaries<1000000007>();
  test_modulus_boundaries<INT_MAX>();

  std::mt19937_64 rng(0x5f3759df12345678ULL);
  test_random<1>(rng, 200);
  test_random<2>(rng, 200);
  test_random<12>(rng, 500);
  test_random<998244353>(rng, 1000);
  test_random<INT_MAX>(rng, 1000);

  return 0;
}
