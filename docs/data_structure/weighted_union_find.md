# Weighted Union Find

各頂点にポテンシャルを持つ無向グラフの連結成分を管理するクラスです。$n$ 頂点のグラフに対して、頂点間のポテンシャル差の制約の追加や、連結判定とポテンシャル差の計算を償却 $O(\alpha(n))$ で行えます。

連結成分ごとに代表となる頂点が 1 つ管理されています。制約の追加により連結成分が合併されるとき、新たな代表元は、合併前の連結成分のうち頂点数が大きい方の代表元になります。

## テンプレート引数

```cpp
template <typename T>
class WeightedUnionFind;
```

ポテンシャルおよびその差を表す型 `T` を指定します。

### 制約

- `T` は `bool` を除く cv 修飾のないオブジェクト型
- `T{}` によって加法の単位元が得られ、`+=` および `-=` によって可換群をなし、`==` によって等値比較ができる

## メンバ型

### UniteResult

```cpp
struct UniteResult {
  bool consistent;
  std::optional<std::pair<int, int>> merged;
};
```

`unite` の実行結果を表す構造体です。

- `consistent`: 追加しようとした制約が既存の制約と矛盾しない場合は `true`、矛盾する場合は `false`
- `merged`: 連結成分が合併された場合は合併後の代表元と吸収された古い代表元のペア、そうでなければ `std::nullopt`

## コンストラクタ

```cpp
WeightedUnionFind::WeightedUnionFind();       // (1)
WeightedUnionFind::WeightedUnionFind(int n);  // (2)
```

- (1): $0$ 頂点 $0$ 辺のグラフで初期化します。
- (2): $n$ 頂点 $0$ 辺のグラフで初期化します。

### 制約

- (2): $n \geq 0$

### 計算量

- (1): $O(1)$
- (2): $O(n)$

## unite

```cpp
UniteResult WeightedUnionFind::unite(int x, int y, const T& w);
```

頂点 $x$ のポテンシャル $p_x$ と頂点 $y$ のポテンシャル $p_y$ に対して、$p_y - p_x = w$ となる制約を追加します。

すでに頂点 $x$ と頂点 $y$ が連結であり既存の制約と矛盾する場合は、状態を変更せず `consistent` が `false` の結果を返します。この操作によって連結成分が合併される場合、新たな代表元は、合併前の連結成分のうち頂点数が大きい方の代表元になります。ただし、頂点数が等しいときは頂点 $x$ が属していた方の代表元になります。

`T` が整数型の場合、ポテンシャルの差は $2^w$ を法として管理されます。ここで $w$ は `T` のビット幅です。

### 制約

- $0 \leq x < n$
- $0 \leq y < n$

### 計算量

- 償却 $O(\alpha(n))$

## find

```cpp
int WeightedUnionFind::find(int x);
```

頂点 $x$ が属する連結成分の代表元を返します。

### 制約

- $0 \leq x < n$

### 計算量

- 償却 $O(\alpha(n))$

## same

```cpp
bool WeightedUnionFind::same(int x, int y);
```

頂点 $x$ と頂点 $y$ が連結であれば `true` を、そうでなければ `false` を返します。

### 制約

- $0 \leq x < n$
- $0 \leq y < n$

### 計算量

- 償却 $O(\alpha(n))$

## diff

```cpp
T WeightedUnionFind::diff(int x, int y);
```

頂点 $x$ のポテンシャル $p_x$ と頂点 $y$ のポテンシャル $p_y$ の差 $p_y - p_x$ を返します。

`T` が整数型の場合、ポテンシャルの差が `T` の範囲を超えても $2^w$ を法として合同な値が返ります。ここで $w$ は `T` のビット幅です。

### 制約

- $0 \leq x < n$
- $0 \leq y < n$
- `same(x, y)` が `true` である

### 計算量

- 償却 $O(\alpha(n))$

## size

```cpp
int WeightedUnionFind::size(int x);
```

頂点 $x$ が属する連結成分の頂点数を返します。

### 制約

- $0 \leq x < n$

### 計算量

- 償却 $O(\alpha(n))$

## count_groups

```cpp
int WeightedUnionFind::count_groups() const;
```

連結成分の個数を返します。

### 計算量

- $O(1)$

## groups

```cpp
std::vector<std::vector<int>> WeightedUnionFind::groups();
```

すべての連結成分について、その連結成分に属する頂点番号の配列をまとめた配列を返します。それぞれの連結成分内の頂点は昇順に格納されますが、集合の順番は未定義です。

### 計算量

- $O(n)$

## 使用例

```cpp
#include <iostream>

#include "library3372/data_structure/weighted_union_find.hpp"

int main() {
  library3372::WeightedUnionFind<long long> uf(4);

  auto r1 = uf.unite(0, 1, 3);
  auto r2 = uf.unite(1, 2, 5);
  std::cout << r1.consistent << " " << r2.merged.has_value() << "\n";  // 1 1

  std::cout << uf.same(0, 2) << "\n";  // 1
  std::cout << uf.diff(0, 2) << "\n";  // 8
  std::cout << uf.diff(2, 0) << "\n";  // -8

  auto r3 = uf.unite(0, 2, 7);
  std::cout << r3.consistent << " " << r3.merged.has_value() << "\n";  // 0 0
  return 0;
}
```
