// Repository: Library3372
// File: library3372/internal/random/xoshiro256.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/internal/random/xoshiro256.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_INTERNAL_RANDOM_XOSHIRO256_HPP_
#define LIBRARY3372_LIBRARY3372_INTERNAL_RANDOM_XOSHIRO256_HPP_

#include <bit>

namespace library3372::internal {

class Xoshiro256 {
 public:
  using result_type = unsigned long long;

  static constexpr result_type min() { return 0; }
  static constexpr result_type max() { return -1ULL; }

  Xoshiro256() : Xoshiro256(1ULL) {}
  explicit Xoshiro256(result_type seed_value) { seed(seed_value); }

  void seed(result_type seed_value) {
    unsigned long long z = seed_value;
    for (int i = 0; i < 4; ++i) {
      z += 0x9e3779b97f4a7c15ULL;
      unsigned long long t = z;
      t = (t ^ (t >> 30)) * 0xbf58476d1ce4e5b9ULL;
      t = (t ^ (t >> 27)) * 0x94d049bb133111ebULL;
      s_[i] = t ^ (t >> 31);
    }
  }

  result_type operator()() {
    const unsigned long long result = std::rotl(s_[1] * 5, 7) * 9;
    const unsigned long long t = s_[1] << 17;

    s_[2] ^= s_[0];
    s_[3] ^= s_[1];
    s_[1] ^= s_[2];
    s_[0] ^= s_[3];

    s_[2] ^= t;
    s_[3] = std::rotl(s_[3], 45);

    return result;
  }

 private:
  unsigned long long s_[4];
};

}  // namespace library3372::internal

#endif  // LIBRARY3372_LIBRARY3372_INTERNAL_RANDOM_XOSHIRO256_HPP_
