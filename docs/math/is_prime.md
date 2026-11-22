# is_prime

整数が素数であるかを判定します。64 bit 符号付き整数全体に対して正確な判定を計算量 $O(\log n)$ で行えます。

## is_prime

```cpp
bool is_prime(long long n);
```

整数 $n$ が素数であれば `true` を、そうでなければ `false` を返します。$n \leq 1$ の場合は `false` を返します。

### 計算量

- $n \leq 1$ のとき $O(1)$
- $n \geq 2$ のとき $O(\log n)$

## 使用例

```cpp
#include <iostream>

#include "library3372/math/is_prime.hpp"

int main() {
  std::cout << library3372::is_prime(1) << "\n";                      // 0
  std::cout << library3372::is_prime(2) << "\n";                      // 1
  std::cout << library3372::is_prime(561) << "\n";                    // 0
  std::cout << library3372::is_prime(998244353) << "\n";              // 1
  std::cout << library3372::is_prime(2305843009213693951LL) << "\n";  // 1
  return 0;
}
```
