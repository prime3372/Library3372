// Repository: Library3372
// File: tests/math/convolution_ll_avx2_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/math/convolution_ll_avx2_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef __AVX2__
#error "AVX2 support (__AVX2__) is required for convolution_ll_avx2_test."
#endif

#include <cassert>

#include "tests/math/convolution_ll_test_common.hpp"

int main() {
  assert(__builtin_cpu_supports("avx2"));
  run_all_convolution_ll_tests();
  return 0;
}
