# 素数判定

整数が素数であるかを判定します。64 bit 符号付き整数全体に対して正確な判定を計算量 $O(\log n)$ で行えます。

## is_prime

```cpp
bool is_prime(long long n);
```

整数 $n$ が素数であれば `true` を、そうでなければ `false` を返します。

### 計算量

- $O(\log n)$

### 補足

$n \geq 4759123141$ のときは、 $n < 4759123141$ のときと比べて定数倍が約 2 ～ 3 倍悪化します。

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
