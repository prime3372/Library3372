// Repository: Library3372
// File: tests/math/convolution_mod_2_64_avx2_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/math/convolution_mod_2_64_avx2_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef __AVX2__
#error "AVX2 is required for convolution_mod_2_64_avx2_test"
#endif

#include "tests/math/convolution_mod_2_64_test_common.hpp"

int main() {
  run_convolution_mod_2_64_tests();
  return 0;
}
