# Fenwick Tree

長さ $n$ の数列 $a_0, a_1, \dots, a_{n-1}$ に対する一点加算と区間和取得を構築 $O(n)$ クエリあたり $O(\log n)$ で処理します。

## テンプレート引数

```cpp
template <typename T>
class FenwickTree;
```

数列の要素の型 `T` を指定します。

### 制約

- `T` は `bool` を除く cv 修飾のないオブジェクト型
- `T{}` によって加法の単位元が得られ、`+=` および `-=` によって可換群をなす

## コンストラクタ

```cpp
FenwickTree::FenwickTree();  // (1)

FenwickTree::FenwickTree(int n);  // (2)

FenwickTree::FenwickTree(const std::vector<T>& a);  // (3)
```

- (1): 長さ $0$ の数列で初期化します。
- (2): 長さ $n$ の数列をすべて単位元 `T{}` で初期化します。
- (3): 配列 `a` を初期値として長さ $n$ の数列を構築します。

### 制約

- (2): $n \geq 0$

### 計算量

- (1): $O(1)$
- (2), (3): $O(n)$

## add

```cpp
void FenwickTree::add(int p, const T& x);
```

$a_p$ に $x$ を加算します。

`T` が整数型の場合、加算後の値が `T` の範囲を超えても $2^w$ を法として合同な値に更新されます。ここで $w$ は `T` のビット幅です。

### 制約

- $0 \leq p < n$

### 計算量

- $O(\log n)$

## set

```cpp
void FenwickTree::set(int p, const T& x);
```

$a_p$ の値を $x$ に変更します。

### 制約

- $0 \leq p < n$

### 計算量

- $O(\log n)$

## sum

```cpp
T FenwickTree::sum(int l, int r) const;
```

$\sum_{i = l}^{r - 1} a_i$ を返します。 $l = r$ のときは `T{}` を返します。

`T` が整数型の場合、総和が `T` の範囲を超えても $2^w$ を法として合同な値が返ります。ここで $w$ は `T` のビット幅です。

### 制約

- $0 \leq l \leq r \leq n$

### 計算量

- $O(\log n)$

## max_right

```cpp
int FenwickTree::max_right(int l, const T& x) const;
```

以下の条件をすべて満たす整数 $r$ を返します。

- $l \leq r \leq n$
- $l < i \leq r$ に対して $\mathrm{sum}(l, i) \leq x$
- $r < i \leq n$ に対して $\mathrm{sum}(l, i) > x$

### 制約

- `T` は整数型
- $0 \leq l \leq n$
- $x \geq 0$
- すべての $l \leq i < n$ に対して $a_i \geq 0$
- $\sum_{i = l}^{n - 1} a_i$ がオーバーフローしない

### 計算量

- $O(\log n)$

## min_left

```cpp
int FenwickTree::min_left(int r, const T& x) const;
```

以下の条件をすべて満たす整数 $l$ を返します。

- $0 \leq l \leq r$
- $l \leq i < r$ に対して $\mathrm{sum}(i, r) \leq x$
- $0 \leq i < l$ に対して $\mathrm{sum}(i, r) > x$

### 制約

- `T` は整数型
- $0 \leq r \leq n$
- $x \geq 0$
- すべての $0 \leq i < r$ に対して $a_i \geq 0$
- $\sum_{i = 0}^{r - 1} a_i$ がオーバーフローしない

### 計算量

- $O(\log n)$

## size

```cpp
int FenwickTree::size() const;
```

数列の長さ $n$ を返します。

### 計算量

- $O(1)$

## 使用例

```cpp
#include <iostream>
#include <vector>

#include "library3372/data_structure/fenwick_tree.hpp"

int main() {
  std::vector<long long> a = {3, 1, 4, 1, 5};
  library3372::FenwickTree<long long> fw(a);

  std::cout << fw.sum(1, 4) << "\n";  // 6
  std::cout << fw.sum(2, 2) << "\n";  // 0

  fw.add(2, 2);
  fw.set(0, 1);
  std::cout << fw.sum(2, 3) << "\n";  // 6
  std::cout << fw.sum(0, 5) << "\n";  // 14

  int r = fw.max_right(0, 8);
  std::cout << r << "\n";  // 3
  return 0;
}
```
