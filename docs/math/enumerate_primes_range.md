# 区間素数列挙

指定された半開区間 $[l, r)$ に含まれるすべての素数を昇順に列挙します。

## enumerate_primes

```cpp
std::vector<long long> enumerate_primes(long long l, long long r);
```

半開区間 $[l, r)$ に含まれるすべての素数を昇順に格納した配列を返します。

### 制約

- $0 \leq l \leq r$

### 計算量

- 時間 $O(\min((r - l + \sqrt{r}) \log \log r, (r - l) \log r))$

## 補足

この関数は `library3372/math/enumerate_primes.hpp` に定義されています。

## 使用例

```cpp
#include <iostream>

#include "library3372/math/enumerate_primes.hpp"

int main() {
  for (long long p : library3372::enumerate_primes(10, 20)) {
    std::cout << p << " ";  // 11 13 17 19
  }
  std::cout << "\n";
  return 0;
}
```
