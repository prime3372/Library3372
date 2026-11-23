# Convolution (mod 2^32)

長さ $n$ と $m$ の整数列に対し、$2^{32}$ を法とする畳み込みの結果を $O((n + m) \log (n + m))$ で求めます。

## convolution_mod_2_32

```cpp
template <typename T = unsigned int>
std::vector<T> convolution_mod_2_32(const std::vector<T>& a,
                                    const std::vector<T>& b);
```

数列 `a` と `b` に対し、長さ $|a| + |b| - 1$ の数列 $c$ を $c_k = \left(\sum_{i + j = k} a_i b_j\right) \bmod 2^{32}$ として計算します。`a` または `b` が空の場合は空の `std::vector` を返します。

### 制約

- $|a| + |b| - 1 \leq 2^{24}$
- `T` は整数型

### 計算量

- $n = |a| + |b|$ として $O(n \log n)$

## 使用例

```cpp
#include <iostream>
#include <vector>

#include "library3372/math/convolution_mod_2_32.hpp"

int main() {
  std::vector<unsigned int> a = {1000000000U, 2000000000U};
  std::vector<unsigned int> b = {3U, 4U};
  for (unsigned int x : library3372::convolution_mod_2_32(a, b)) {
    std::cout << x << " ";
  }
  std::cout << "\n";
  // 3000000000 1410065408 3705032704

  return 0;
}
```
