# 商列挙

非負整数 $n$ に対して $\lfloor n / x \rfloor$ がとり得るすべての値とその値に対応する $x$ の範囲を $O(\sqrt{n})$ で列挙します。

## enumerate_quotients

```cpp
std::vector<std::tuple<long long, long long, long long>> enumerate_quotients(
    long long n);
```

$1 \leq x \leq n$ における $\lfloor n / x \rfloor$ のとり得るすべての値 $q$ と、 $\lfloor n / x \rfloor = q$ となる半開区間 $[l, r)$ の組 $(q, l, r)$ を $q$ の昇順で格納した配列を返します。 $n = 0$ のときは空配列を返します。

### 制約

- $0 \leq n \leq 2^{63} - 2$

### 計算量

- $O(\sqrt{n})$

### 補足

$n \geq 2^{32}$ のときは、 $n < 2^{32}$ のときと比べて定数倍が約 1.5 ～ 2 倍悪化します。

## 使用例

```cpp
#include <iostream>

#include "library3372/math/enumerate_quotients.hpp"

int main() {
  for (auto [q, l, r] : library3372::enumerate_quotients(6)) {
    std::cout << q << ": [" << l << ", " << r << ")\n";
  }
  // 1: [4, 7)
  // 2: [3, 4)
  // 3: [2, 3)
  // 6: [1, 2)
  return 0;
}
```
