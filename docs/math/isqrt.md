# 整数平方根

非負整数 $n$ に対して $\lfloor \sqrt{n} \rfloor$ を計算量 $O(1)$ で求めます。

## isqrt

```cpp
template <typename T>
T isqrt(T n);
```

非負整数 $n$ に対して $\lfloor \sqrt{n} \rfloor$ を返します。

### 制約

- `T` は整数型
- $n \geq 0$

### 計算量

- $O(1)$

## 使用例

```cpp
#include <iostream>

#include "library3372/math/isqrt.hpp"

int main() {
  std::cout << library3372::isqrt(0) << "\n";                      // 0
  std::cout << library3372::isqrt(8) << "\n";                      // 2
  std::cout << library3372::isqrt(9) << "\n";                      // 3
  std::cout << library3372::isqrt(1000000000000000000LL) << "\n";  // 1000000000
  return 0;
}
```
