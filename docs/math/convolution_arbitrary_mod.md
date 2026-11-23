# Convolution Arbitrary Mod

任意の法による 2 つの数列の剰余環上の畳み込みを計算する関数です。長さ $n$ と $m$ の数列に対し、畳み込みの結果を $O((n + m) \log (n + m))$ で求めます。

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
                                         int mod);  // (3)

template <int m, typename T>
std::vector<T> convolution_arbitrary_mod(const std::vector<T>& a,
                                         const std::vector<T>& b);  // (4)
```

数列 `a` と `b` に対し、長さ $|a| + |b| - 1$ の数列 $c$ を $c_k = \left(\sum_{i + j = k} a_i b_j\right) \bmod M$ として計算し、返します。ここで、法 $M$ は (1), (4) では $m$、(2) では `DynamicModInt<id>::mod()`、(3) では `mod` です。`a` または `b` が空の場合は空の `std::vector` を返します。

### 制約

- (1), (4): $m \geq 1$
- (3): $\mathrm{mod} \geq 1$
- $|a| + |b| - 1 \leq 2^{24}$
- (3), (4): `T` は整数型

### 計算量

- $n = |a| + |b|$ として $O(n \log n)$

## 使用例

```cpp
#include <iostream>
#include <vector>

#include "library3372/math/convolution_arbitrary_mod.hpp"
#include "library3372/math/dynamic_modint.hpp"
#include "library3372/math/static_modint.hpp"

int main() {
  using SMint = library3372::StaticModInt<1000000007>;
  std::vector<SMint> a = {SMint(1), SMint(2), SMint(3), SMint(4)};
  std::vector<SMint> b = {SMint(1), SMint(2), SMint(4)};
  for (SMint x : library3372::convolution_arbitrary_mod(a, b)) {
    std::cout << x << " ";
  }
  std::cout << "\n";
  // 1 4 11 18 20 16

  using DMint = library3372::DynamicModInt<0>;
  DMint::set_mod(1000000000);
  std::vector<DMint> da = {DMint(500000000), DMint(500000000)};
  std::vector<DMint> db = {DMint(3), DMint(4)};
  for (DMint x : library3372::convolution_arbitrary_mod(da, db)) {
    std::cout << x << " ";
  }
  std::cout << "\n";
  // 500000000 500000000 0

  std::vector<int> ia = {100000, 200000};
  std::vector<int> ib = {300000, 400000};
  for (int x : library3372::convolution_arbitrary_mod(ia, ib, 1000000007)) {
    std::cout << x << " ";
  }
  std::cout << "\n";
  // 999999797 999999307 999999447

  return 0;
}
```
