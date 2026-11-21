// Repository: Library3372
// File: benchmarks/graph/max_flow/generators/layered_deep.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/graph/max_flow/generators/layered_deep.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <random>

int main(int argc, char* argv[]) {
  assert(argc >= 4);
  int layers = std::atoi(argv[1]);
  int width = std::atoi(argv[2]);
  long long max_cap = std::atoll(argv[3]);
  assert(layers >= 2 && width >= 1 && max_cap >= 1);

  int n = layers * width + 2;
  int s = 0;
  int t = n - 1;
  int m = 2 * width + (layers - 1) * width * width;

  std::mt19937_64 rng(0x082efa98ec4e6c89ULL);
  std::uniform_int_distribution<long long> cdist(1, max_cap);

  std::cout << n << ' ' << m << ' ' << s << ' ' << t << '\n';
  for (int j = 0; j < width; ++j) {
    std::cout << s << ' ' << (1 + j) << ' ' << cdist(rng) << '\n';
  }
  for (int l = 0; l + 1 < layers; ++l) {
    int base_u = 1 + l * width;
    int base_v = 1 + (l + 1) * width;
    for (int i = 0; i < width; ++i) {
      for (int j = 0; j < width; ++j) {
        std::cout << (base_u + i) << ' ' << (base_v + j) << ' ' << cdist(rng)
                  << '\n';
      }
    }
  }
  int last_base = 1 + (layers - 1) * width;
  for (int i = 0; i < width; ++i) {
    std::cout << (last_base + i) << ' ' << t << ' ' << cdist(rng) << '\n';
  }
  return 0;
}
