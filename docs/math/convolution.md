# Convolution

剰余環上の畳み込みを計算する関数です。長さ $n$ と $m$ の整数列に対し、畳み込みの結果を $O((n + m) \log (n + m) + \log \mathrm{mod})$ で求めます。法は NTT-friendly な素数でなければいけません。

## convolution

```cpp
template <int m>
std::vector<StaticModInt<m>> convolution(
    const std::vector<StaticModInt<m>>& a,
    const std::vector<StaticModInt<m>>& b);  // (1)

template <long long m = 998244353, typename T>
std::vector<T> convolution(const std::vector<T>& a,
                           const std::vector<T>& b);  // (2)
```

数列 `a` と `b` に対し、長さ $|a| + |b| - 1$ の数列 $c$ を $c_k = \left(\sum_{i + j = k} a_i b_j\right) \bmod m$ として計算します。`a` または `b` が空の場合は空の `std::vector` を返します。

### 制約

- (1): $2 \leq m \leq 2 \times 10^9$
- (2): $m \geq 2$
- $m$ は素数
- $2^c \mid (m - 1)$ かつ $|a| + |b| - 1 \leq 2^c$ を満たす整数 $c$ が存在する
- (2): `T` は整数型

### 計算量

- $n = |a| + |b|$ として $O(n \log n + \log m)$

## 使用例

```cpp
#include <iostream>
#include <vector>

#include "library3372/math/convolution.hpp"
#include "library3372/math/static_modint.hpp"

int main() {
  using Mint = library3372::StaticModInt<998244353>;

  std::vector<Mint> a = {1, 2, 3, 4};
  std::vector<Mint> b = {1, 2, 4};
  std::vector<Mint> c = library3372::convolution(a, b);
  for (Mint x : c) std::cout << x << " ";
  std::cout << "\n";
  // 1 4 11 18 20 16

  std::vector<int> ai = {1, 2, 3, 4};
  std::vector<int> bi = {1, 2, 4};
  std::vector<int> ci = library3372::convolution(ai, bi);
  for (int x : ci) std::cout << x << " ";
  std::cout << "\n";
  // 1 4 11 18 20 16

  return 0;
}
```
