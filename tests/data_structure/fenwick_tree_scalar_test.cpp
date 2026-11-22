// Repository: Library3372
// File: tests/data_structure/fenwick_tree_scalar_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/data_structure/fenwick_tree_scalar_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#pragma GCC push_options
#pragma GCC target("no-avx2")

#ifdef __AVX2__
#error "AVX2 must be disabled to test the Scalar implementation of FenwickTree."
#endif

#include <random>

#include "tests/data_structure/fenwick_tree_test_common.hpp"

int main() {
  std::mt19937_64 rng(0x517cc1b727220a95ULL);
  run_common_fenwick_tree_tests(rng);
  return 0;
}

#pragma GCC pop_options
