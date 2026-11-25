# Disjoint Sparse Table

長さ $n$ の静的な数列 $a_0, a_1, \dots, a_{n-1}$ に対するモノイドの区間積の取得を構築 $O(n \log n)$ クエリあたり $O(1)$ で処理します。

## テンプレート引数

```cpp
template <typename S, auto op, auto e>
class DisjointSparseTable;
```

モノイド $(S, \mathrm{op}, e)$ を指定します。簡略化のため $\mathrm{op}(x, y)$ を $xy$ と表記します。

### 制約

- $(S, \mathrm{op}, e)$ はモノイドをなす
- `S` は cv 修飾のないコピー可能なオブジェクト型
- `op` は `op(const S&, const S&)` の形で呼び出し可能であり、`S` に変換可能な値を返す
- `e` は `e()` の形で呼び出し可能であり、`S` に変換可能な値を返す

## コンストラクタ

```cpp
DisjointSparseTable::DisjointSparseTable();                         // (1)
DisjointSparseTable::DisjointSparseTable(const std::vector<S>& a);  // (2)
```

- (1): 長さ $0$ の数列で初期化します。
- (2): 配列 `a` を初期値として長さ $n$ のデータ構造を構築します。

### 計算量

- (1): $O(1)$
- (2): $O(n \log n)$

## get

```cpp
S DisjointSparseTable::get(int p) const;
```

$a_p$ を返します。

### 制約

- $0 \leq p < n$

### 計算量

- $O(1)$

## prod

```cpp
S DisjointSparseTable::prod(int l, int r) const;
```

$a_l a_{l+1} \cdots a_{r-1}$ を返します。 $l = r$ のときは `e()` を返します。

### 制約

- $0 \leq l \leq r \leq n$

### 計算量

- $O(1)$

## all_prod

```cpp
S DisjointSparseTable::all_prod() const;
```

$a_0a_1 \cdots a_{n-1}$ を返します。 $n = 0$ のときは `e()` を返します。

### 計算量

- $O(1)$

## size

```cpp
int DisjointSparseTable::size() const;
```

数列の長さ $n$ を返します。

### 計算量

- $O(1)$

## 補足

$(S, \mathrm{op}, e)$ が厳密なモノイドをなさない場合でも、以下の条件をすべて満たすときは正しく動作します。

- 任意の連続部分列 $a_l, a_{l+1}, \dots, a_{r-1}$ について $a_l a_{l+1} \cdots a_{r-1} \in S$
- $e \in S$
- 任意の $x \in S$ について $ex = xe = x$
- 任意の $x, y, z \in S$ について $xy, yz, (xy)z, x(yz) \in S$ ならば $(xy)z = x(yz)$

## 使用例

```cpp
#include <iostream>
#include <vector>

#include "library3372/data_structure/disjoint_sparse_table.hpp"

int main() {
  constexpr auto op = [](long long a, long long b) { return a + b; };
  constexpr auto e = []() { return 0LL; };

  std::vector<long long> a = {3, 1, 4, 1, 5};
  library3372::DisjointSparseTable<long long, op, e> dst(a);

  std::cout << dst.prod(1, 4) << "\n";   // 6
  std::cout << dst.prod(2, 2) << "\n";   // 0
  std::cout << dst.get(4) << "\n";       // 5
  std::cout << dst.all_prod() << "\n";   // 14
  return 0;
}
```
