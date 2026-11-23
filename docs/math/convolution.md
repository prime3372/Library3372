# Convolution

2 つの数列の畳み込みを計算する関数群です。長さ $n$ と $m$ の数列に対し、畳み込みの結果を $O((n + m) \log (n + m))$ で求めます。

## convolution

```cpp
template <int m>
std::vector<StaticModInt<m>> convolution(
    const std::vector<StaticModInt<m>>& a,
    const std::vector<StaticModInt<m>>& b);  // (1)

template <int m = 998244353, typename T>
std::vector<T> convolution(const std::vector<T>& a,
                           const std::vector<T>& b);  // (2)
```

数列 `a` と `b` に対し、長さ $|a| + |b| - 1$ の数列 $c$ を $c_k = \left(\sum_{i + j = k} a_i b_j\right) \bmod m$ として計算し、返します。`a` または `b` が空の場合は空の `std::vector` を返します。

### 制約

- $2 \leq m \leq 2 \times 10^9$
- $m$ は素数
- $2^c \mid (m - 1)$ かつ $|a| + |b| - 1 \leq 2^c$ を満たす整数 $c$ が存在する
- (2): $T$ は整数型

### 計算量

- $n = |a| + |b|$ として $O(n \log n + \log m)$

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

#include "library3372/math/convolution.hpp"
#include "library3372/math/static_modint.hpp"

int main() {
  using Mint = library3372::StaticModInt<998244353>;

  std::vector<Mint> a = {Mint(1), Mint(2), Mint(3), Mint(4)};
  std::vector<Mint> b = {Mint(1), Mint(2), Mint(4)};
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

  std::vector<long long> al = {1000000000LL, 1000000000LL};
  std::vector<long long> bl = {1000000000LL, 1000000000LL};
  std::vector<long long> cl = library3372::convolution_ll(al, bl);
  for (long long x : cl) std::cout << x << " ";
  std::cout << "\n";
  // 1000000000000000000 2000000000000000000 1000000000000000000

  return 0;
}
```
