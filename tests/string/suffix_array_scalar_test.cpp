// Repository: Library3372
// File: tests/string/suffix_array_scalar_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/string/suffix_array_scalar_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#pragma GCC push_options
#pragma GCC target("no-avx2")
#undef __AVX2__

#include "tests/string/suffix_array_test_common.hpp"

int main() {
  run_all_suffix_array_tests();
  return 0;
}

#pragma GCC pop_options
