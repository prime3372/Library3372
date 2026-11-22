// Repository: Library3372
// File: tests/string/suffix_array_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/string/suffix_array_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifdef __AVX2__
#pragma GCC push_options
#pragma GCC target("no-avx2")
#define LIBRARY3372_TEST_RESTORE_AVX2_
#endif

#include "library3372/string/suffix_array.hpp"

#ifdef LIBRARY3372_TEST_RESTORE_AVX2_
#pragma GCC pop_options
#undef LIBRARY3372_TEST_RESTORE_AVX2_
#endif

#include "tests/string/suffix_array_test_common.hpp"

int main() {
  run_all_suffix_array_tests();
  return 0;
}
