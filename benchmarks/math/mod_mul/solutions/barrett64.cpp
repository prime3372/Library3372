// Repository: Library3372
// File: benchmarks/math/mod_mul/solutions/barrett64.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/math/mod_mul/solutions/barrett64.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <iostream>
#include <vector>

#include "library3372/internal/chrono/timer.hpp"
#include "library3372/internal/hash/hash_combine.hpp"

namespace {

class Barrett64 {
 public:
  explicit Barrett64(unsigned long long mod) : mod_(mod) {
    unsigned __int128 im = (-(unsigned __int128)mod) / mod + 1;
    im_hi_ = (unsigned long long)(im >> 64);
    im_lo_ = (unsigned long long)im;
  }

  unsigned long long mul(unsigned long long a, unsigned long long b) const {
    unsigned __int128 z = (unsigned __int128)a * b;
    unsigned long long z_hi = (unsigned long long)(z >> 64);
    unsigned long long z_lo = (unsigned long long)z;
    unsigned long long q =
        z_hi * im_hi_ +
        (unsigned long long)(((unsigned __int128)z_hi * im_lo_) >> 64) +
        (unsigned long long)(((unsigned __int128)z_lo * im_hi_) >> 64);
    unsigned __int128 res = z - (unsigned __int128)q * mod_;
    while (res >= mod_) res -= mod_;
    return (unsigned long long)res;
  }

 private:
  unsigned long long mod_;
  unsigned long long im_hi_;
  unsigned long long im_lo_;
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
  for (int i = 0; i < q; ++i) {
    std::cin >> queries[i].m >> queries[i].x >> queries[i].y >> queries[i].k;
  }

  library3372::internal::Timer timer;
  std::vector<unsigned long long> ans(2 * q);
  for (int i = 0; i < q; ++i) {
    Barrett64 bt(queries[i].m);
    unsigned long long a = queries[i].x;
    unsigned long long b = queries[i].y;
    int k = queries[i].k;
    for (int j = 0; j < k; ++j) {
      a = bt.mul(a, b);
      b = bt.mul(b, a);
    }
    ans[2 * i] = a;
    ans[2 * i + 1] = b;
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
