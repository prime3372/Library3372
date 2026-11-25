# Convolution (long long)

64 bit 整数列に対する畳み込みの結果を $O(n \log n)$ で求めます。

## convolution_ll

```cpp
std::vector<long long> convolution_ll(const std::vector<long long>& a,
                                      const std::vector<long long>& b);
```

数列 $a, b$ に対し、長さ $\lvert a \rvert + \lvert b \rvert - 1$ の数列 $c$ を $c_k = \sum_{i + j = k} a_i b_j$ として計算します。 $a$ または $b$ が空の場合は空の `std::vector` を返します。

### 制約

- $\lvert a \rvert + \lvert b \rvert - 1 \leq 2^{24}$
- 出力の各要素を $c_k$ として $\lvert c_k \rvert \leq 2 \times 10^{18}$

### 計算量

- $n = \lvert a \rvert + \lvert b \rvert$ として $O(n \log n)$

## 使用例

```cpp
#include <iostream>
#include <vector>

#include "library3372/math/convolution_ll.hpp"

int main() {
  std::vector<long long> a = {1000000000LL, 1000000000LL};
  std::vector<long long> b = {1000000000LL, 1000000000LL};
  std::vector<long long> c = library3372::convolution_ll(a, b);
  for (long long x : c) std::cout << x << " ";
  std::cout << "\n";
  // 1000000000000000000 2000000000000000000 1000000000000000000

  return 0;
}
```
