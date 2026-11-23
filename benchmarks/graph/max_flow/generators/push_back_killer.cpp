// Repository: Library3372
// File: benchmarks/graph/max_flow/generators/push_back_killer.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/benchmarks/graph/max_flow/generators/push_back_killer.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include <cassert>
#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[]) {
  assert(argc >= 3);
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int gadgets = std::atoi(argv[1]);
  long long scale = std::atoll(argv[2]);
  assert(gadgets >= 1 && scale >= 2);

  long long cap = scale / 2;
  long long half1 = cap / 2;
  long long half2 = cap - half1;

  int n = 2 + 6 * gadgets;
  int s = 0;
  int t = n - 1;
  int m = 13 * gadgets;

  std::cout << n << ' ' << m << ' ' << s << ' ' << t << '\n';
  for (int g = 0; g < gadgets; ++g) {
    int base = 1 + 6 * g;
    int u = base;
    int v = base + 1;
    int x1 = base + 2;
    int x2 = base + 3;
    int y1 = base + 4;
    int y2 = base + 5;

    // Shortest path s -> u -> v -> t (length 3) must later be cancelled via
    // reverse edge v -> u.
    std::cout << s << ' ' << u << ' ' << cap << '\n';
    std::cout << u << ' ' << v << ' ' << cap << '\n';
    std::cout << v << ' ' << t << ' ' << cap << '\n';

    // Longer bypasses (length 4) with parallel edges, self-loops, and 0-cap
    // edges.
    std::cout << s << ' ' << x1 << ' ' << half1 << '\n';
    std::cout << s << ' ' << x1 << ' ' << half2 << '\n';
    std::cout << x1 << ' ' << x2 << ' ' << cap << '\n';
    std::cout << x2 << ' ' << v << ' ' << cap << '\n';

    std::cout << u << ' ' << y1 << ' ' << cap << '\n';
    std::cout << y1 << ' ' << y2 << ' ' << cap << '\n';
    std::cout << y2 << ' ' << t << ' ' << cap << '\n';

    std::cout << u << ' ' << u << ' ' << cap << '\n';
    std::cout << v << ' ' << v << ' ' << cap << '\n';
    std::cout << x1 << ' ' << y1 << " 0\n";
  }
  return 0;
}
