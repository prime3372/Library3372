# Convolution Arbitrary Mod

剰余環上の畳み込みを $O(n \log n)$ で計算する関数です。

## convolution_arbitrary_mod

```cpp
template <int m>
std::vector<StaticModInt<m>> convolution_arbitrary_mod(
    const std::vector<StaticModInt<m>>& a,
    const std::vector<StaticModInt<m>>& b);  // (1)

template <int id>
std::vector<DynamicModInt<id>> convolution_arbitrary_mod(
    const std::vector<DynamicModInt<id>>& a,
    const std::vector<DynamicModInt<id>>& b);  // (2)

template <typename T>
std::vector<T> convolution_arbitrary_mod(const std::vector<T>& a,
                                         const std::vector<T>& b,
                                         long long m);  // (3)

template <long long m, typename T>
std::vector<T> convolution_arbitrary_mod(const std::vector<T>& a,
                                         const std::vector<T>& b);  // (4)
```

数列 $a, b$ に対し、長さ $\lvert a \rvert + \lvert b \rvert - 1$ の数列 $c$ を $c_k = \left(\sum_{i + j = k} a_i b_j\right) \bmod m$ として計算します。 $a$ または $b$ が空の場合は空の `std::vector` を返します。

### 制約

- (1), (3), (4): $m \geq 1$
- $\lvert a \rvert + \lvert b \rvert - 1 \leq 2^{25}$
- (3), (4): `T` は整数型

### 計算量

- $n = \lvert a \rvert + \lvert b \rvert$ として $O(n \log n)$

## 使用例

```cpp
#include <iostream>
#include <vector>

#include "library3372/math/convolution_arbitrary_mod.hpp"
#include "library3372/math/dynamic_modint.hpp"
#include "library3372/math/static_modint.hpp"

int main() {
  using Mint = library3372::StaticModInt<1000000007>;

  std::vector<Mint> a = {1, 2, 3, 4};
  std::vector<Mint> b = {1, 2, 4};
  for (Mint x : library3372::convolution_arbitrary_mod(a, b)) {
    std::cout << x << " ";
  }
  std::cout << "\n";
  // 1 4 11 18 20 16

  return 0;
}
```
