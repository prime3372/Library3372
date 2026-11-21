// Repository: Library3372
// File: library3372/internal/hash/hash_combine.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/internal/hash/hash_combine.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_INTERNAL_HASH_HASH_COMBINE_HPP_
#define LIBRARY3372_LIBRARY3372_INTERNAL_HASH_HASH_COMBINE_HPP_

namespace library3372::internal {

inline unsigned long long hash_combine(unsigned long long seed,
                                       unsigned long long value) {
  return seed ^ (value + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2));
}

}  // namespace library3372::internal

#endif  // LIBRARY3372_LIBRARY3372_INTERNAL_HASH_HASH_COMBINE_HPP_
