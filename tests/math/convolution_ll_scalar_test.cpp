// Repository: Library3372
// File: tests/math/convolution_ll_scalar_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/math/convolution_ll_scalar_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#pragma GCC push_options
#pragma GCC target("no-avx2")

#ifdef __AVX2__
#error \
    "AVX2 must be disabled to test the Scalar implementation of convolution_ll."
#endif

#include "tests/math/convolution_ll_test_common.hpp"

int main() {
  run_all_convolution_ll_tests();
  return 0;
}

#pragma GCC pop_options
