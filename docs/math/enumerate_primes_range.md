# 区間素数列挙

指定された半開区間 $[l, r)$ に含まれるすべての素数を昇順に列挙します。

## enumerate_primes_range

```cpp
std::vector<long long> enumerate_primes_range(long long l, long long r);
```

半開区間 $[l, r)$ に含まれるすべての素数を昇順に格納した配列を返します。

### 制約

- $0 \leq l \leq r$

### 計算量

- 時間 $O(\min((r - l + \sqrt{r}) \log \log r, (r - l) \log r))$

## 使用例

```cpp
#include <iostream>

#include "library3372/math/enumerate_primes_range.hpp"

int main() {
  for (long long p : library3372::enumerate_primes_range(10, 20)) {
    std::cout << p << " ";  // 11 13 17 19
  }
  std::cout << "\n";
  return 0;
}
```
