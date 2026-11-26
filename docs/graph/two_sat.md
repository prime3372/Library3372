# 2-SAT

$n$ 個の真偽値変数 $x_0, x_1, \dots, x_{n-1}$ に対する 2-SAT 問題を計算量 $O(n + m)$ で解くクラスです。 $m$ 個の節 $(x_i = f) \lor (x_j = g)$ を追加し、すべての節を同時に満たす割り当てが存在するかを判定できます。

## コンストラクタ

```cpp
TwoSat::TwoSat();       // (1)
TwoSat::TwoSat(int n);  // (2)
```

- (1): $0$ 変数 $0$ 節の 2-SAT で初期化します。
- (2): $n$ 変数 $0$ 節の 2-SAT で初期化します。

### 制約

- (2): $n \geq 0$

### 計算量

- (1): $O(1)$
- (2): $O(n)$

## add_clause

```cpp
void TwoSat::add_clause(int i, bool f, int j, bool g);
```

節 $(x_i = f) \lor (x_j = g)$ を追加します。

### 制約

- $0 \leq i < n$
- $0 \leq j < n$

### 計算量

- 償却 $O(1)$

## satisfiable

```cpp
bool TwoSat::satisfiable();
```

追加されたすべての節を同時に満たす変数の割り当てが存在する場合は `true`、存在しない場合は `false` を返します。存在する場合はその割り当てを 1 つ構築します。

### 計算量

- $O(n + m)$

## answer

```cpp
const std::vector<bool>& TwoSat::answer() const;
```

直前の `satisfiable()` の呼び出しで構築された割り当てを長さ $n$ の配列として返します。

### 制約

- `satisfiable()` を 1 回以上呼んでいる
- 直前に `satisfiable()` を呼んでから `add_clause()` を 1 回も呼んでいない
- 直前の `satisfiable()` の返り値が `true` である

### 計算量

- $O(1)$

### 補足

`add_clause()` や `satisfiable()` を呼び出すと、以前に取得した `answer()` の参照は無効になります。

## 使用例

```cpp
#include <iostream>

#include "library3372/graph/two_sat.hpp"

int main() {
  library3372::TwoSat ts(3);
  ts.add_clause(0, true, 1, false);
  ts.add_clause(1, true, 2, true);
  ts.add_clause(2, false, 0, false);

  if (ts.satisfiable()) {
    const auto& ans = ts.answer();
    for (int i = 0; i < 3; ++i) {
      std::cout << ans[i] << (i == 2 ? "\n" : " ");
    }
    // 0 0 1
  }
  return 0;
}
```
