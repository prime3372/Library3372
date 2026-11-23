# Convolution (mod 2^64)

長さ $n$ と $m$ の整数列に対し、$2^{64}$ を法とする畳み込みの結果を $O((n + m) \log (n + m))$ で求めます。

## convolution_mod_2_64

```cpp
template <typename T = unsigned long long>
std::vector<T> convolution_mod_2_64(const std::vector<T>& a,
                                    const std::vector<T>& b);
```

数列 `a` と `b` に対し、長さ $|a| + |b| - 1$ の数列 $c$ を $c_k = \left(\sum_{i + j = k} a_i b_j\right) \bmod 2^{64}$ として計算します。`a` または `b` が空の場合は空の `std::vector` を返します。

### 制約

- $|a| + |b| - 1 \leq 2^{24}$
- `T` は整数型

### 計算量

- $n = |a| + |b|$ として $O(n \log n)$

## 使用例

```cpp
#include <iostream>
#include <vector>

#include "library3372/math/convolution_mod_2_64.hpp"

int main() {
  std::vector<unsigned long long> a = {10000000000000000000ULL, 1ULL};
  std::vector<unsigned long long> b = {2ULL, 3ULL};
  for (unsigned long long x : library3372::convolution_mod_2_64(a, b)) {
    std::cout << x << " ";
  }
  std::cout << "\n";
  // 1553255926290448384 11553255926290448386 3

  return 0;
}
```
