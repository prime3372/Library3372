// Repository: Library3372
// File: library3372/internal/random/xorshift.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/internal/random/xorshift.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_INTERNAL_RANDOM_XORSHIFT_HPP_
#define LIBRARY3372_LIBRARY3372_INTERNAL_RANDOM_XORSHIFT_HPP_

namespace library3372::internal {

class Xorshift {
 public:
  using result_type = unsigned long long;

  static constexpr result_type min() { return 0; }
  static constexpr result_type max() { return -1ULL; }

  Xorshift() : Xorshift(88172645463325252ULL) {}
  explicit Xorshift(result_type seed_value) : state_(seed_value) {}

  void seed(result_type seed_value) { state_ = seed_value; }

  result_type operator()() {
    state_ ^= state_ << 13;
    state_ ^= state_ >> 7;
    state_ ^= state_ << 17;
    return state_;
  }

 private:
  unsigned long long state_;
};

}  // namespace library3372::internal

#endif  // LIBRARY3372_LIBRARY3372_INTERNAL_RANDOM_XORSHIFT_HPP_
