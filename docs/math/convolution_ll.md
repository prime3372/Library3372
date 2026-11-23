# Convolution LL

2 つの 64 bit 整数列の畳み込みを計算する関数です。長さ $n$ と $m$ の数列に対し、畳み込みの結果を $O((n + m) \log (n + m))$ で求めます。

## convolution_ll

```cpp
std::vector<long long> convolution_ll(const std::vector<long long>& a,
                                      const std::vector<long long>& b);
```

数列 `a` と `b` に対し、長さ $|a| + |b| - 1$ の数列 $c$ を $c_k = \sum_{i + j = k} a_i b_j$ として計算し、返します。`a` または `b` が空の場合は空の `std::vector` を返します。

### 制約

- $|a| + |b| - 1 \leq 2^{24}$
- 出力の各要素を $c_k$ として $|c_k| \leq 2252081290784276480$

### 計算量

- $n = |a| + |b|$ として $O(n \log n)$

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
