# Union Find

無向グラフの連結成分を管理するクラスです。$n$ 頂点のグラフに対して、無向辺の追加や連結判定などを償却 $O(\alpha(n))$ で行えます。

連結成分ごとに代表となる頂点が 1 つ管理されています。辺の追加により連結成分が合併されるとき、新たな代表元は、合併前の連結成分のうち頂点数が大きい方の代表元になります。

## コンストラクタ

```cpp
UnionFind::UnionFind();       // (1)
UnionFind::UnionFind(int n);  // (2)
```

- (1): $0$ 頂点 $0$ 辺の無向グラフで初期化します。
- (2): $n$ 頂点 $0$ 辺の無向グラフで初期化します。

### 制約

- (2): $n \geq 0$

### 計算量

- (1): $O(1)$
- (2): $O(n)$

## unite

```cpp
std::optional<std::pair<int, int>> UnionFind::unite(int x, int y);
```

頂点 $x$ と頂点 $y$ の間に無向辺を追加します。

この操作によって連結成分が合併される場合、新たな代表元は、合併前の連結成分のうち頂点数が大きい方の代表元になります。ただし、頂点数が等しいときは頂点 $x$ が属していた方の代表元になります。

戻り値として以下を返します。
- 連結成分が合併された場合は (合併後の代表元, 吸収された古い代表元) のペア
- そうでなければ `std::nullopt`

### 制約

- $0 \leq x < n$
- $0 \leq y < n$

### 計算量

- 償却 $O(\alpha(n))$

## find

```cpp
int UnionFind::find(int x);
```

頂点 $x$ が属する連結成分の代表元を返します。

### 制約

- $0 \leq x < n$

### 計算量

- 償却 $O(\alpha(n))$

## same

```cpp
bool UnionFind::same(int x, int y);
```

頂点 $x$ と頂点 $y$ が連結であれば `true` を、そうでなければ `false` を返します。

### 制約

- $0 \leq x < n$
- $0 \leq y < n$

### 計算量

- 償却 $O(\alpha(n))$

## size

```cpp
int UnionFind::size(int x);
```

頂点 $x$ が属する連結成分の頂点数を返します。

### 制約

- $0 \leq x < n$

### 計算量

- 償却 $O(\alpha(n))$

## count_groups

```cpp
int UnionFind::count_groups() const;
```

連結成分の個数を返します。

### 計算量

- $O(1)$

## groups

```cpp
std::vector<std::vector<int>> UnionFind::groups();
```

すべての連結成分について「その連結成分に属する頂点番号の配列」をまとめた配列を返します。それぞれの連結成分内の頂点は昇順に格納されますが、集合の順番は未定義です。

### 計算量

- $O(n)$

## 使用例

```cpp
#include <iostream>

#include "library3372/data_structure/union_find.hpp"

int main() {
  library3372::UnionFind uf(5);

  std::cout << uf.count_groups() << "\n";  // 5

  auto merged = uf.unite(0, 1);
  std::cout << merged.has_value() << "\n";  // 1
  std::cout << uf.same(0, 1) << "\n";       // 1
  std::cout << uf.size(0) << "\n";          // 2
  std::cout << uf.count_groups() << "\n";   // 4

  auto redundant = uf.unite(0, 1);
  std::cout << redundant.has_value() << "\n";  // 0
  return 0;
}
```
