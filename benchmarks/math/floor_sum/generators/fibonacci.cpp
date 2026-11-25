// Repository: Library3372
// File: benchmarks/math/floor_sum/generators/fibonacci.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/floor_sum/generators/fibonacci.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cassert>
#include <climits>
#include <cstdlib>
#include <iostream>
#include <random>
#include <vector>

int main(int argc, char* argv[]) {
  assert(argc >= 2);
  int q = std::atoi(argv[1]);
  assert(q >= 1);

  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::vector<long long> fib = {1, 2};
  while (fib.back() <= LLONG_MAX - fib[fib.size() - 2]) {
    fib.push_back(fib.back() + fib[fib.size() - 2]);
  }
  const int f_cnt = (int)fib.size();

  std::mt19937_64 rng(0x7c8f3a12e5b904d1ULL);
  std::uniform_int_distribution<int> dist_idx(f_cnt - 10, f_cnt - 1);
  std::uniform_int_distribution<long long> dist_n(1, LLONG_MAX);
  std::uniform_int_distribution<long long> dist_b(0, fib.back() - 1);

  std::cout << q << '\n';
  for (int i = 0; i < q; ++i) {
    int idx = dist_idx(rng);
    long long m = fib[idx];
    long long a = fib[idx - 1];
    long long n = dist_n(rng);
    long long b = dist_b(rng) % m;
    std::cout << n << ' ' << m << ' ' << a << ' ' << b << '\n';
  }
  return 0;
}
