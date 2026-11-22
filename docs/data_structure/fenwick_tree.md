# Fenwick Tree

長さ $n$ の数列 $a = (a_0, a_1, \dots, a_{n-1})$ を管理するデータ構造です。要素への加算・代入や区間和の計算、区間和に関する二分探索を $O(\log n)$ で行えます。また、任意の初期配列からの構築を $O(n)$ で行えます。

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

template <typename U>
FenwickTree::FenwickTree(const std::vector<U>& a);  // (4)
```

- (1): 長さ $0$ の数列で初期化します。
- (2): 長さ $n$ の数列をすべて単位元 `T{}` で初期化します。
- (3), (4): 配列 `a` の各要素を `T` に変換した列を初期値として長さ $n$ の数列を構築します。

### 制約

- (2): $n \geq 0$
- (4): `U` は `T` と異なる型であり、`T(x)` の形式で `const U&` から `T` を構築できる

### 計算量

- (1): $O(1)$
- (2), (3), (4): $O(n)$

## size

```cpp
int FenwickTree::size() const;
```

数列の長さ $n$ を返します。

### 計算量

- $O(1)$

## add

```cpp
void FenwickTree::add(int p, const T& x);
```

$a_p$ に $x$ を加算します。

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

## get

```cpp
T FenwickTree::get(int p) const;
```

$a_p$ の値を返します。

### 制約

- $0 \leq p < n$

### 計算量

- $O(\log n)$

## sum

```cpp
T FenwickTree::sum(int r) const;  // (1)

T FenwickTree::sum(int l, int r) const;  // (2)
```

- (1): $\sum_{i = 0}^{r - 1} a_i$ を返します。$r = 0$ のときは `T{}` を返します。
- (2): $\sum_{i = l}^{r - 1} a_i$ を返します。$l = r$ のときは `T{}` を返します。

### 制約

- (1): $0 \leq r \leq n$
- (2): $0 \leq l \leq r \leq n$

### 計算量

- $O(\log n)$

## max_right

```cpp
template <typename F>
int FenwickTree::max_right(F pred) const;  // (1)

template <typename F>
int FenwickTree::max_right(int l, F pred) const;  // (2)
```

以下の条件をすべて満たす整数 $r$ を返します。

- (1): $0 \leq r \leq n$
  - `pred(sum(r))` が `true` となる
  - $r < n$ ならば `pred(sum(r + 1))` が `false` となる
- (2): $l \leq r \leq n$
  - `pred(sum(l, r))` が `true` となる
  - $r < n$ ならば `pred(sum(l, r + 1))` が `false` となる

`pred` が単調性を満たす場合、条件を満たす $r$ は一意に定まります。

### 制約

- (1): すべての $0 \leq r \leq n$ に対して `pred(sum(r))` の戻り値が広義単調減少であり、`pred(T{})` が `true`
- (2): $0 \leq l \leq n$
- (2): すべての $l \leq r \leq n$ に対して `pred(sum(l, r))` の戻り値が広義単調減少であり、`pred(T{})` が `true`

### 計算量

- $O(\log n)$

## min_left

```cpp
template <typename F>
int FenwickTree::min_left(int r, F pred) const;
```

以下の条件をすべて満たす整数 $l$ を返します。

- $0 \leq l \leq r$
- `pred(sum(l, r))` が `true` となる
- $l > 0$ ならば `pred(sum(l - 1, r))` が `false` となる

`pred` が単調性を満たす場合、条件を満たす $l$ は一意に定まります。

### 制約

- $0 \leq r \leq n$
- すべての $0 \leq l \leq r$ に対して `pred(sum(l, r))` の戻り値が広義単調増加であり、`pred(T{})` が `true`

### 計算量

- $O(\log n)$

## 使用例

```cpp
#include <iostream>
#include <vector>

#include "library3372/data_structure/fenwick_tree.hpp"

int main() {
  std::vector<long long> a = {3, 1, 4, 1, 5};
  library3372::FenwickTree<long long> ft(a);

  std::cout << ft.sum(1, 4) << "\n";  // 6

  ft.add(2, 2);
  ft.set(0, 1);
  std::cout << ft.get(2) << "\n";     // 6
  std::cout << ft.sum(5) << "\n";     // 14

  int r = ft.max_right([](long long s) { return s <= 8; });
  std::cout << r << "\n";  // 3
  return 0;
}
```
