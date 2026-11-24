// Repository: Library3372
// File: tests/math/enumerate_quotients_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/math/enumerate_quotients_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include "library3372/math/enumerate_quotients.hpp"

#include <cassert>
#include <random>
#include <tuple>
#include <vector>

namespace {

using library3372::enumerate_quotients;

constexpr bool verify_quotients_constexpr(long long n) {
  auto res = enumerate_quotients(n);
  if (n == 0) return res.empty();
  if (res.empty()) return false;
  if (std::get<2>(res.front()) != n + 1) return false;
  if (std::get<1>(res.back()) != 1) return false;
  long long prev_q = 0;
  long long prev_l = n + 1;
  for (const auto& [q, l, r] : res) {
    if (l >= r) return false;
    if (r != prev_l) return false;
    if (q <= prev_q) return false;
    if (n / l != q || n / (r - 1) != q) return false;
    if (r <= n && n / r >= q) return false;
    if (l > 1 && n / (l - 1) <= q) return false;
    prev_q = q;
    prev_l = l;
  }
  return true;
}

constexpr bool test_constexpr() {
  using Vec = std::vector<std::tuple<long long, long long, long long>>;
  if (!enumerate_quotients(0).empty()) return false;
  if (enumerate_quotients(1) != Vec{{1, 1, 2}}) return false;
  if (enumerate_quotients(2) != Vec{{1, 2, 3}, {2, 1, 2}}) return false;
  if (enumerate_quotients(3) != Vec{{1, 2, 4}, {3, 1, 2}}) return false;
  if (enumerate_quotients(4) != Vec{{1, 3, 5}, {2, 2, 3}, {4, 1, 2}}) {
    return false;
  }
  if (enumerate_quotients(5) != Vec{{1, 3, 6}, {2, 2, 3}, {5, 1, 2}}) {
    return false;
  }
  if (enumerate_quotients(6) !=
      Vec{{1, 4, 7}, {2, 3, 4}, {3, 2, 3}, {6, 1, 2}}) {
    return false;
  }
  if (enumerate_quotients(12) !=
      Vec{{1, 7, 13}, {2, 5, 7}, {3, 4, 5}, {4, 3, 4}, {6, 2, 3}, {12, 1, 2}}) {
    return false;
  }
  for (long long n : {9999LL, 10000LL, 10001LL}) {
    if (!verify_quotients_constexpr(n)) return false;
  }
  return true;
}

std::vector<std::tuple<long long, long long, long long>>
naive_enumerate_quotients(long long n) {
  std::vector<std::tuple<long long, long long, long long>> res;
  for (long long r = n + 1; r > 1;) {
    long long x = r - 1;
    long long q = n / x;
    long long l = x;
    while (l > 1 && n / (l - 1) == q) --l;
    res.emplace_back(q, l, r);
    r = l;
  }
  return res;
}

void verify_quotients(long long n) {
  auto res = enumerate_quotients(n);
  if (n == 0) {
    assert(res.empty());
    return;
  }
  assert(!res.empty());
  assert(std::get<0>(res.front()) == 1);
  assert(std::get<2>(res.front()) == n + 1);
  assert(std::get<0>(res.back()) == n);
  assert(std::get<1>(res.back()) == 1);

  long long prev_q = 0;
  long long prev_l = n + 1;
  for (const auto& [q, l, r] : res) {
    assert(1 <= l && l < r && r <= n + 1);
    assert(r == prev_l);
    assert(q > prev_q);
    assert((__int128)q * l <= n && n < (__int128)(q + 1) * l);
    assert((__int128)q * (r - 1) <= n && n < (__int128)(q + 1) * (r - 1));
    prev_q = q;
    prev_l = l;
  }
}

void test_exhaustive_small() {
  static_assert(test_constexpr());

  for (long long n = 0; n <= 2000; ++n) {
    assert(enumerate_quotients(n) == naive_enumerate_quotients(n));
  }
}

void test_special_cases() {
  // Around squares and k*(k + 1) across 32-bit and 64-bit boundaries
  const long long ks[] = {1LL, 2LL, 3LL, 10LL, 100LL, 1000LL, 65535LL, 65536LL};
  for (long long k : ks) {
    for (long long d = -2; d <= 2; ++d) {
      if (k * k + d >= 0) verify_quotients(k * k + d);
      if (k * (k + 1) + d >= 0) verify_quotients(k * (k + 1) + d);
    }
  }
  for (long long d = -1; d <= 1; ++d) {
    verify_quotients(100000LL * 100000LL + d);
    verify_quotients(100000LL * 100001LL + d);
  }

  // Powers of 2 up to 2^34 (including 2^32 threshold)
  for (int e = 0; e <= 34; ++e) {
    long long p = 1LL << e;
    for (long long d = -1; d <= 1; ++d) {
      if (p + d >= 0) verify_quotients(p + d);
    }
  }
}

void test_random(std::mt19937_64& rng) {
  std::uniform_int_distribution<long long> small_dist(0LL, 10000LL);
  for (int i = 0; i < 100; ++i) {
    long long n = small_dist(rng);
    assert(enumerate_quotients(n) == naive_enumerate_quotients(n));
  }

  std::uniform_int_distribution<long long> mid_dist(100001LL, 10000000LL);
  for (int i = 0; i < 10; ++i) verify_quotients(mid_dist(rng));

  std::uniform_int_distribution<long long> u32_dist(10000001LL, 4294967295LL);
  for (int i = 0; i < 5; ++i) verify_quotients(u32_dist(rng));

  std::uniform_int_distribution<long long> u64_dist(4294967296LL,
                                                    10000000000LL);
  for (int i = 0; i < 3; ++i) verify_quotients(u64_dist(rng));
}

}  // namespace

int main() {
  test_exhaustive_small();
  test_special_cases();

  std::mt19937_64 rng(0x9e3779b97f4a7c15ULL);
  test_random(rng);
  return 0;
}
