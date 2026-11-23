// Repository: Library3372
// File: tests/string/suffix_array_test_common.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/string/suffix_array_test_common.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_TESTS_STRING_SUFFIX_ARRAY_TEST_COMMON_HPP_
#define LIBRARY3372_TESTS_STRING_SUFFIX_ARRAY_TEST_COMMON_HPP_

#include <algorithm>
#include <cassert>
#include <functional>
#include <limits>
#include <numeric>
#include <random>
#include <string>
#include <string_view>
#include <vector>

#include "library3372/string/suffix_array.hpp"

namespace {

template <typename T, typename Compare = std::less<>>
std::vector<int> naive_sa_vec(const std::vector<T>& s,
                              Compare compare = Compare{}) {
  int n = (int)s.size();
  std::vector<int> sa(n);
  std::iota(sa.begin(), sa.end(), 0);
  std::sort(sa.begin(), sa.end(), [&](int l, int r) {
    int limit = n - (l > r ? l : r);
    for (int k = 0; k < limit; ++k) {
      if (compare(s[l + k], s[r + k])) return true;
      if (compare(s[r + k], s[l + k])) return false;
    }
    return l > r;
  });
  return sa;
}

std::vector<int> naive_sa_str(std::string_view s) {
  int n = (int)s.size();
  std::vector<int> sa(n);
  std::iota(sa.begin(), sa.end(), 0);
  std::sort(sa.begin(), sa.end(),
            [&](int l, int r) { return s.substr(l) < s.substr(r); });
  return sa;
}

struct LessOnly {
  int a;
  int b;
  bool operator<(const LessOnly& other) const {
    if (a != other.a) return a < other.a;
    return b < other.b;
  }
};

struct NoOrder {
  int x;
  int y;
};

bool cmp_int_desc(int a, int b) { return a > b; }

void test_empty_and_tiny() {
  assert(library3372::suffix_array(std::string_view("")).empty());
  assert(library3372::suffix_array(std::string("")).empty());
  assert(library3372::suffix_array("").empty());
  assert(library3372::suffix_array(std::vector<int>{}, 0).empty());
  assert(library3372::suffix_array(std::vector<int>{}).empty());
  assert(library3372::suffix_array(std::vector<bool>{}).empty());
  assert(library3372::suffix_array(std::vector<long long>{}).empty());
  assert(library3372::suffix_array(std::vector<LessOnly>{}).empty());

  assert((library3372::suffix_array("a") == std::vector<int>{0}));
  assert(
      (library3372::suffix_array(std::vector<int>{42}) == std::vector<int>{0}));
  assert((library3372::suffix_array(std::vector<int>{0}, 0) ==
          std::vector<int>{0}));
  assert((library3372::suffix_array(std::vector<bool>{true}) ==
          std::vector<int>{0}));

  assert((library3372::suffix_array("ab") == std::vector<int>{0, 1}));
  assert((library3372::suffix_array("ba") == std::vector<int>{1, 0}));
  assert((library3372::suffix_array("aa") == std::vector<int>{1, 0}));
  assert((library3372::suffix_array(std::vector<int>{1, 1}, 1) ==
          std::vector<int>{1, 0}));

  std::string_view sv = "abracadabra";
  assert(library3372::suffix_array(sv) == naive_sa_str(sv));
  assert(library3372::suffix_array(std::string(sv)) == naive_sa_str(sv));
  assert(library3372::suffix_array("mississippi") ==
         naive_sa_str("mississippi"));
}

void test_signed_and_unsigned_bytes() {
  std::string raw;
  for (int i = 0; i < 256; ++i) raw.push_back((char)(unsigned char)i);
  for (int i = 255; i >= 0; --i) raw.push_back((char)(unsigned char)i);
  assert(library3372::suffix_array(raw) == naive_sa_str(raw));

  std::vector<char> vc(raw.begin(), raw.end());
  assert(library3372::suffix_array(vc) == naive_sa_vec(vc));

  std::vector<signed char> vsc(raw.begin(), raw.end());
  assert(library3372::suffix_array(vsc) == naive_sa_vec(vsc));

  std::vector<unsigned char> vuc(raw.begin(), raw.end());
  assert(library3372::suffix_array(vuc) == naive_sa_vec(vuc));
}

void test_threshold_and_exhaustive_small() {
  // Exhaustive binary strings up to length 10.
  for (int n = 1; n <= 10; ++n) {
    for (int mask = 0; mask < (1 << n); ++mask) {
      std::string s(n, 'a');
      std::vector<int> v(n);
      std::vector<bool> vb(n);
      for (int i = 0; i < n; ++i) {
        int bit = (mask >> i) & 1;
        s[i] = (char)('a' + bit);
        v[i] = bit;
        vb[i] = (bit != 0);
      }
      auto expected = naive_sa_str(s);
      assert(library3372::suffix_array(s) == expected);
      assert(library3372::suffix_array(v, 1) == expected);
      assert(library3372::suffix_array(v) == expected);
      assert(library3372::suffix_array(vb) == expected);
    }
  }

  // Lengths around threshold_naive (32) and AVX2 block boundaries (32, 64, 96).
  std::mt19937_64 rng(0x123456789abcdef0ULL);
  for (int n : {30, 31, 32, 33, 34, 35, 63, 64, 65, 66, 95, 96, 97}) {
    for (int trial = 0; trial < 12; ++trial) {
      std::string s(n, 'a');
      std::vector<int> v(n);
      for (int i = 0; i < n; ++i) {
        int x = (int)(rng() % 3);
        s[i] = (char)('a' + x);
        v[i] = x;
      }
      auto expected = naive_sa_str(s);
      assert(library3372::suffix_array(s) == expected);
      assert(library3372::suffix_array(v, 2) == expected);
      assert(library3372::suffix_array(v) == expected);
    }
  }
}

void test_special_patterns() {
  for (int n : {1, 2, 10, 33, 100, 300}) {
    std::string all_eq(n, 'x');
    assert(library3372::suffix_array(all_eq) == naive_sa_str(all_eq));

    std::vector<int> inc(n), dec(n);
    for (int i = 0; i < n; ++i) {
      inc[i] = i;
      dec[i] = n - 1 - i;
    }
    assert(library3372::suffix_array(inc, n - 1) == naive_sa_vec(inc));
    assert(library3372::suffix_array(inc) == naive_sa_vec(inc));
    assert(library3372::suffix_array(dec, n - 1) == naive_sa_vec(dec));
    assert(library3372::suffix_array(dec) == naive_sa_vec(dec));
  }

  // Fibonacci word (deep LMS recursion).
  {
    std::string a = "a", b = "b";
    while ((int)b.size() < 400) {
      std::string c = b + a;
      a = std::move(b);
      b = std::move(c);
    }
    assert(library3372::suffix_array(b) == naive_sa_str(b));
  }

  // Thue-Morse sequence.
  {
    int n = 400;
    std::string tm(n, 'a');
    for (int i = 0; i < n; ++i) {
      tm[i] = (__builtin_popcount((unsigned int)i) & 1) ? 'b' : 'a';
    }
    assert(library3372::suffix_array(tm) == naive_sa_str(tm));
  }

  // Alternating / periodic with trailing sentinel-like tail: [1, 0, 1, 0, 1, 0].
  {
    std::vector<int> v{1, 0, 1, 0, 1, 0};
    assert(library3372::suffix_array(v, 1) == naive_sa_vec(v));
    std::vector<int> v2(200);
    for (int i = 0; i < 200; ++i) v2[i] = (i & 1) ? 0 : 1;
    assert(library3372::suffix_array(v2, 1) == naive_sa_vec(v2));
  }

  // Long LMS substrings around SIMD comparison boundaries (7, 8, 9, 15, 16, 17, 25).
  for (int period : {7, 8, 9, 15, 16, 17, 25}) {
    std::string s;
    std::vector<int> v;
    for (int rep = 0; rep < 6; ++rep) {
      s.push_back('c');
      v.push_back(300);
      for (int k = 0; k < period - 2; ++k) {
        char ch = (rep % 2 == 0 && k == period - 3) ? 'b' : 'a';
        int iv = (rep % 2 == 0 && k == period - 3) ? 1 : 0;
        s.push_back(ch);
        v.push_back(iv);
      }
      s.push_back('d');
      v.push_back(301);
    }
    assert(library3372::suffix_array(s) == naive_sa_str(s));
    assert(library3372::suffix_array(v, 301) == naive_sa_vec(v));
  }
}

void test_integer_types_and_radix_paths() {
  std::mt19937_64 rng(0x9e3779b97f4a7c15ULL);

  // Sparse upper in suffix_array(s, upper).
  {
    std::vector<int> s(150);
    for (int& x : s) x = (int)(rng() % 50000);
    assert(library3372::suffix_array(s, 50000) == naive_sa_vec(s));
  }

  // upper <= 255 with n >= 4096 fast path and int-only path with upper >= 256.
  {
    int n = 5000;
    std::string str(n, 'a');
    std::vector<int> vec(n);
    for (int i = 0; i < n; ++i) {
      int c = (int)(rng() % 5);
      str[i] = (char)('a' + c);
      vec[i] = c;
    }
    auto sa_s = library3372::suffix_array(str);
    auto sa_v = library3372::suffix_array(vec, 4);
    auto sa_g = library3372::suffix_array(vec);
    assert(sa_s == sa_v);
    assert(sa_s == sa_g);
  }

  // Radix sort path with dup_count <= 256 (mostly distinct) and > 256.
  for (int mod : {200, 1000000000}) {
    std::vector<int> s(400);
    for (int& x : s) x = (int)(rng() % (unsigned int)mod) - mod / 2;
    assert(library3372::suffix_array(s) == naive_sa_vec(s));
  }
  {
    // n = 600 with 300 values repeated twice -> dup_count == 300 > 256.
    std::vector<int> s(600);
    for (int i = 0; i < 600; ++i) s[i] = (i % 300) * 100000;
    std::shuffle(s.begin(), s.end(), rng);
    assert(library3372::suffix_array(s) == naive_sa_vec(s));
  }

  // Extreme signed and unsigned integer bounds.
  {
    std::vector<short> vs{std::numeric_limits<short>::min(),
                          std::numeric_limits<short>::max(),
                          -1,
                          0,
                          1,
                          std::numeric_limits<short>::min()};
    vs.resize(300, 0);
    for (int i = 6; i < 300; ++i) vs[i] = (short)(rng());
    assert(library3372::suffix_array(vs) == naive_sa_vec(vs));

    std::vector<unsigned short> vus(300);
    for (int i = 0; i < 300; ++i) vus[i] = (unsigned short)(rng());
    vus[0] = 0;
    vus[1] = std::numeric_limits<unsigned short>::max();
    assert(library3372::suffix_array(vus) == naive_sa_vec(vus));

    std::vector<unsigned int> vui(300);
    for (int i = 0; i < 300; ++i) vui[i] = (unsigned int)rng();
    vui[0] = 0;
    vui[1] = std::numeric_limits<unsigned int>::max();
    assert(library3372::suffix_array(vui) == naive_sa_vec(vui));

    std::vector<long long> vll(300);
    for (int i = 0; i < 300; ++i) vll[i] = (long long)rng();
    vll[0] = std::numeric_limits<long long>::min();
    vll[1] = std::numeric_limits<long long>::max();
    assert(library3372::suffix_array(vll) == naive_sa_vec(vll));

    std::vector<unsigned long long> vull(300);
    for (int i = 0; i < 300; ++i) vull[i] = rng();
    vull[0] = 0;
    vull[1] = std::numeric_limits<unsigned long long>::max();
    assert(library3372::suffix_array(vull) == naive_sa_vec(vull));

    std::vector<__int128> vi128(300);
    for (int i = 0; i < 300; ++i) {
      vi128[i] = (((__int128)(long long)rng()) << 64) | rng();
    }
    assert(library3372::suffix_array(vi128) == naive_sa_vec(vi128));

    std::vector<unsigned __int128> vu128(300);
    for (int i = 0; i < 300; ++i) {
      vu128[i] = (((unsigned __int128)rng()) << 64) | rng();
    }
    assert(library3372::suffix_array(vu128) == naive_sa_vec(vu128));
  }
}

void test_non_integer_types() {
  std::mt19937_64 rng(0xfeedfacecafebeefULL);

  std::vector<double> vd(150);
  for (int i = 0; i < 150; ++i) {
    vd[i] = (double)(rng() % 10) * 0.25 - 1.0;
  }
  vd[0] = -0.0;
  vd[1] = 0.0;
  assert(library3372::suffix_array(vd) == naive_sa_vec(vd));

  std::vector<std::string> vstr(120);
  for (int i = 0; i < 120; ++i) {
    vstr[i] = std::string(1 + (rng() % 3), (char)('a' + (rng() % 3)));
  }
  assert(library3372::suffix_array(vstr) == naive_sa_vec(vstr));

  std::vector<LessOnly> vlo(120);
  for (int i = 0; i < 120; ++i) {
    vlo[i] = LessOnly{(int)(rng() % 4), (int)(rng() % 3)};
  }
  assert(library3372::suffix_array(vlo) == naive_sa_vec(vlo));
}

void test_random_medium() {
  std::mt19937_64 rng(0x243f6a8885a308d3ULL);
  for (int n : {50, 100, 250}) {
    for (int sigma : {2, 4, 26, 200, 300}) {
      std::string s(n, 'a');
      std::vector<int> v(n);
      for (int i = 0; i < n; ++i) {
        int c = (int)(rng() % (unsigned int)sigma);
        s[i] = (char)(unsigned char)c;
        v[i] = c;
      }
      assert(library3372::suffix_array(v, sigma - 1) == naive_sa_vec(v));
      assert(library3372::suffix_array(v) == naive_sa_vec(v));
      if (sigma <= 256) {
        assert(library3372::suffix_array(s) == naive_sa_str(s));
      }
    }
  }
}

void test_custom_compare() {
  std::mt19937_64 rng(0x517cc1b727220a95ULL);

  // Overload disambiguation with non-int upper (calls (1), not (3)).
  {
    std::vector<int> v{2, 0, 1, 2};
    auto expected = naive_sa_vec(v);
    assert(library3372::suffix_array(v, 2LL) == expected);
    assert(library3372::suffix_array(v, 2U) == expected);
    assert(library3372::suffix_array(v, (short)2) == expected);
  }

  // Empty, n = 1, n = 2, small (n <= 32), boundary, and medium with custom
  // comparators.
  for (int n : {0, 1, 2, 10, 32, 33, 64, 150}) {
    std::vector<int> v(n);
    for (int& x : v) x = (int)(rng() % 50) - 25;
    assert(library3372::suffix_array(v, std::less<>{}) == naive_sa_vec(v));
    assert(library3372::suffix_array(v, std::less<int>{}) == naive_sa_vec(v));
    assert(library3372::suffix_array(v, std::greater<>{}) ==
           naive_sa_vec(v, std::greater<>{}));
    assert(library3372::suffix_array(v, cmp_int_desc) ==
           naive_sa_vec(v, cmp_int_desc));

    auto mod_cmp = [](int a, int b) {
      int ma = ((a % 5) + 5) % 5;
      int mb = ((b % 5) + 5) % 5;
      return ma < mb;
    };
    assert(library3372::suffix_array(v, mod_cmp) == naive_sa_vec(v, mod_cmp));

    // All-equivalent under custom comparator.
    auto all_eq_cmp = [](int, int) { return false; };
    assert(library3372::suffix_array(v, all_eq_cmp) ==
           naive_sa_vec(v, all_eq_cmp));
  }

  // std::vector<bool> with custom comparator.
  for (int n : {0, 1, 2, 10, 32, 33, 80}) {
    std::vector<bool> vb(n);
    for (int i = 0; i < n; ++i) vb[i] = ((rng() & 1) != 0);
    assert(library3372::suffix_array(vb, std::greater<>{}) ==
           naive_sa_vec(vb, std::greater<>{}));
  }

  // Type without operator< sorted via custom comparator.
  for (int n : {0, 1, 2, 20, 33, 120}) {
    std::vector<NoOrder> vn(n);
    for (auto& e : vn) e = NoOrder{(int)(rng() % 4), (int)(rng() % 3)};
    auto cmp_x_only = [](const NoOrder& a, const NoOrder& b) {
      return a.x < b.x;
    };
    auto cmp_rev = [](const NoOrder& a, const NoOrder& b) {
      if (a.x != b.x) return a.x > b.x;
      return a.y < b.y;
    };
    assert(library3372::suffix_array(vn, cmp_x_only) ==
           naive_sa_vec(vn, cmp_x_only));
    assert(library3372::suffix_array(vn, cmp_rev) == naive_sa_vec(vn, cmp_rev));
  }
}

void run_all_suffix_array_tests() {
  test_empty_and_tiny();
  test_signed_and_unsigned_bytes();
  test_threshold_and_exhaustive_small();
  test_special_patterns();
  test_integer_types_and_radix_paths();
  test_non_integer_types();
  test_random_medium();
  test_custom_compare();
}

}  // namespace

#endif  // LIBRARY3372_TESTS_STRING_SUFFIX_ARRAY_TEST_COMMON_HPP_
