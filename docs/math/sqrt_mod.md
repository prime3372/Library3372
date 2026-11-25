# 平方剰余

素数 $p$ を法として $x^2 \equiv a \pmod p$ を満たす $x$ を期待 $O(\log p)$ で求めます。

## sqrt_mod

```cpp
std::optional<long long> sqrt_mod(long long a, long long p);
```

$x^2 \equiv a \pmod p$ を満たす最小の非負整数 $x$ が存在する場合はその値を返し、存在しない場合は `std::nullopt` を返します。

### 制約

- $p$ は素数

### 計算量

- 期待 $O(\log p)$

## 使用例

```cpp
#include <iostream>
#include <optional>

#include "library3372/math/sqrt_mod.hpp"

int main() {
  auto x = library3372::sqrt_mod(5, 11);
  if (x) {
    std::cout << *x << "\n";  // 4
  } else {
    std::cout << "None\n";
  }

  auto y = library3372::sqrt_mod(2, 11);
  if (y) {
    std::cout << *y << "\n";
  } else {
    std::cout << "None\n";  // None
  }
  return 0;
}
```
