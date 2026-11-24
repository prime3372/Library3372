# 素数列挙

非負整数 $n$ 以下のすべての素数を計算量 $O(n \log \log n)$ で昇順に列挙します。

## enumerate_primes

```cpp
std::vector<int> enumerate_primes(int n);
```

$n$ 以下のすべての素数を昇順に格納した配列を返します。

### 制約

- $n \geq 0$

### 計算量

- 時間 $O(n \log \log n)$ 空間 $O(n / \log n)$

## 使用例

```cpp
#include <iostream>

#include "library3372/math/enumerate_primes.hpp"

int main() {
  for (int p : library3372::enumerate_primes(10)) {
    std::cout << p << " ";  // 2 3 5 7
  }
  std::cout << "\n";
  return 0;
}
```
