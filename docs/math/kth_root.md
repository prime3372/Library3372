# 整数累乗根

非負整数 $n$ と正整数 $k$ に対して $\lfloor n^{1/k} \rfloor$ を計算量 $O(\log \min(k, \log n))$ で求めます。

## kth_root

```cpp
template <typename T, typename K>
T kth_root(T n, K k);
```

非負整数 $n$ と正整数 $k$ に対して $\lfloor n^{1/k} \rfloor$ を返します。

### 制約

- `T` は整数型
- `K` は整数型
- $n \geq 0$
- $k \geq 1$

### 計算量

- $O(\log \min(k, \log n))$

## 使用例

```cpp
#include <iostream>

#include "library3372/math/kth_root.hpp"

int main() {
  std::cout << library3372::kth_root(0, 3) << "\n";                      // 0
  std::cout << library3372::kth_root(26, 3) << "\n";                     // 2
  std::cout << library3372::kth_root(27, 3) << "\n";                     // 3
  std::cout << library3372::kth_root(1000000000000000000LL, 3) << "\n";  // 1000000
  std::cout << library3372::kth_root(1000000000000000000LL, 6) << "\n";  // 1000
  return 0;
}
```
