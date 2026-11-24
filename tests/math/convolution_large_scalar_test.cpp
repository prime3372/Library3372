// Repository: Library3372
// File: tests/math/convolution_large_scalar_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/math/convolution_large_scalar_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifdef __AVX2__
#pragma GCC target("no-avx2")
#undef __AVX2__
#endif

#include "tests/math/convolution_large_test_common.hpp"

int main() {
  run_convolution_large_tests();
  return 0;
}
