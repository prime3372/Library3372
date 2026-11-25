# Floor Sum

$1$ 次式の床関数に関する総和 $\sum_{i = 0}^{n - 1} \lfloor (a i + b) / m \rfloor$ を $O(\log m)$ で求めます。

## floor_sum

```cpp
long long floor_sum(long long n, long long m, long long a, long long b);
```

$\sum_{i = 0}^{n - 1} \lfloor (a i + b) / m \rfloor$ を返します。計算結果が `long long` の範囲を超える場合は $2^{64}$ を法として合同な値を返します。

### 制約

- $0 \leq n \leq 2^{63} - 1$
- $1 \leq m \leq 2^{63} - 1$
- $-2^{63} \leq a, b \leq 2^{63} - 1$

### 計算量

- $O(\log m)$

## 使用例

```cpp
#include <iostream>

#include "library3372/math/floor_sum.hpp"

int main() {
  std::cout << library3372::floor_sum(4, 10, 6, 3) << "\n";  // 3
  std::cout << library3372::floor_sum(6, 5, 4, 3) << "\n";   // 13
  std::cout << library3372::floor_sum(4, 3, -2, 1) << "\n";  // -4
  std::cout << library3372::floor_sum(0, 1, 0, 0) << "\n";   // 0
  return 0;
}
```
