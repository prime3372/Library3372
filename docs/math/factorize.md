# 素因数分解

正の整数 $n$ を期待計算量 $O(n^{1/4})$ で素因数分解します。

## factorize

```cpp
std::vector<std::pair<long long, int>> factorize(long long n);
```

正の整数 $n$ を素因数分解し、素因数と指数のペアを素因数の昇順で格納した配列を返します。 $n = 1$ のときは空配列を返します。

### 制約

- $n \geq 1$

### 計算量

- 期待 $O(n^{1/4})$

## 使用例

```cpp
#include <iostream>

#include "library3372/math/factorize.hpp"

int main() {
  for (auto [p, e] : library3372::factorize(12)) {
    std::cout << p << "^" << e << " ";  // 2^2 3^1
  }
  std::cout << "\n";
  return 0;
}
```
