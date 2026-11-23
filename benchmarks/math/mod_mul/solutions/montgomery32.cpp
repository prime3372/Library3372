// Repository: Library3372
// File: benchmarks/math/mod_mul/solutions/montgomery32.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/mod_mul/solutions/montgomery32.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <iostream>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"

namespace {

class Montgomery32 {
 public:
  explicit Montgomery32(unsigned int mod)
      : mod_(mod),
        inv_(mod),
        r2_((unsigned int)((-(unsigned long long)mod) % mod)) {
    for (int i = 0; i < 4; ++i) inv_ *= 2U - mod_ * inv_;
  }

  unsigned int to_mont(unsigned int x) const {
    return reduce((unsigned long long)x * r2_);
  }

  unsigned int from_mont(unsigned int x) const { return reduce(x); }

  unsigned int mul(unsigned int a, unsigned int b) const {
    return reduce((unsigned long long)a * b);
  }

 private:
  unsigned int reduce(unsigned long long t) const {
    unsigned int m = (unsigned int)t * inv_;
    unsigned int high_t = (unsigned int)(t >> 32);
    unsigned int high_mn = (unsigned int)(((unsigned long long)m * mod_) >> 32);
    return high_t < high_mn ? high_t - high_mn + mod_ : high_t - high_mn;
  }

  unsigned int mod_;
  unsigned int inv_;
  unsigned int r2_;
};

}  // namespace

struct Query {
  unsigned long long m;
  unsigned long long x;
  unsigned long long y;
  int k;
};

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int q;
  std::cin >> q;
  std::vector<Query> queries(q);
  bool overflow = false;
  for (int i = 0; i < q; ++i) {
    std::cin >> queries[i].m >> queries[i].x >> queries[i].y >> queries[i].k;
    if (queries[i].m > 0xffffffffULL) overflow = true;
  }
  if (overflow) {
    std::cout << "skip M >= 2^32\n";
    return 0;
  }

  library3372::internal::Timer timer;
  std::vector<unsigned long long> ans(2 * q);
  for (int i = 0; i < q; ++i) {
    Montgomery32 mont((unsigned int)queries[i].m);
    unsigned int a = mont.to_mont((unsigned int)queries[i].x);
    unsigned int b = mont.to_mont((unsigned int)queries[i].y);
    int k = queries[i].k;
    for (int j = 0; j < k; ++j) {
      a = mont.mul(a, b);
      b = mont.mul(b, a);
    }
    ans[2 * i] = mont.from_mont(a);
    ans[2 * i + 1] = mont.from_mont(b);
  }
  double run_sec = timer.elapsed();

  unsigned long long ans_hash = 0;
  for (unsigned long long v : ans) {
    ans_hash = library3372::internal::hash_combine(ans_hash, v);
  }

  std::cout << "hash " << ans_hash << '\n';
  std::cout << "time run " << run_sec << '\n';
  return 0;
}
