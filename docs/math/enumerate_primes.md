# 素数列挙

指定された範囲に含まれる素数を昇順に列挙します。$n$ 以下の素数列挙および区間 $[l, r)$ の素数列挙に対応しています。

## enumerate_primes

```cpp
std::vector<int> enumerate_primes(int n);                           // (1)
std::vector<long long> enumerate_primes(long long l, long long r);  // (2)
```

- (1): $n$ 以下のすべての素数を昇順に格納した配列を返します。
- (2): 半開区間 $[l, r)$ に含まれるすべての素数を昇順に格納した配列を返します。

### 制約

- (1): $n \geq 0$
- (2): $0 \leq l \leq r$

### 計算量

- (1): 時間 $O(n \log \log n)$ 空間 $O(n / \log n)$
- (2): 時間 $O(\min((r - l + \sqrt{r}) \log \log r, (r - l) \log r))$

## 使用例

```cpp
#include <iostream>

#include "library3372/math/enumerate_primes.hpp"

int main() {
  for (int p : library3372::enumerate_primes(10)) {
    std::cout << p << " ";  // 2 3 5 7
  }
  std::cout << "\n";

  for (long long p : library3372::enumerate_primes(10, 20)) {
    std::cout << p << " ";  // 11 13 17 19
  }
  std::cout << "\n";
  return 0;
}
```
