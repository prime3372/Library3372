# 原始根

素数 $p$ に対して、最小の原始根を求めます。

## primitive_root

```cpp
long long primitive_root(long long p);
```

素数 $p$ に対する最小の原始根を返します。

### 制約

- $p$ は素数

### 計算量

- 最小の原始根を $g$ として、期待 $O(p^{1/4} + g \log^2 p)$

## 使用例

```cpp
#include <iostream>

#include "library3372/math/primitive_root.hpp"

int main() {
  std::cout << library3372::primitive_root(2) << "\n";           // 1
  std::cout << library3372::primitive_root(998244353) << "\n";   // 3
  std::cout << library3372::primitive_root(1000000007) << "\n";  // 5
  return 0;
}
```
