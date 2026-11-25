# Dual Fenwick Tree

長さ $n$ の数列 $a_0, a_1, \dots, a_{n-1}$ に対する区間加算と一点取得を構築 $O(n)$ クエリあたり $O(\log n)$ で処理します。

## テンプレート引数

```cpp
template <typename T>
class DualFenwickTree;
```

数列の要素の型 `T` を指定します。

### 制約

- `T` は `bool` を除く cv 修飾のないオブジェクト型
- `T{}` によって加法の単位元が得られ、`+=` および `-=` によって可換群をなす

## コンストラクタ

```cpp
DualFenwickTree::DualFenwickTree();                         // (1)
DualFenwickTree::DualFenwickTree(int n);                    // (2)
DualFenwickTree::DualFenwickTree(const std::vector<T>& a);  // (3)
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
void DualFenwickTree::add(int p, const T& x);         // (1)
void DualFenwickTree::add(int l, int r, const T& x);  // (2)
```

- (1): $a_p$ に $x$ を加算します。
- (2): すべての $l \leq i < r$ に対して $a_i$ に $x$ を加算します。

`T` が整数型の場合、加算後の値が `T` の範囲を超えても $2^w$ を法として合同な値に更新されます。ここで $w$ は `T` のビット幅です。

### 制約

- (1): $0 \leq p < n$
- (2): $0 \leq l \leq r \leq n$

### 計算量

- $O(\log n)$

## set

```cpp
void DualFenwickTree::set(int p, const T& x);
```

$a_p$ の値を $x$ に変更します。

### 制約

- $0 \leq p < n$

### 計算量

- $O(\log n)$

## get

```cpp
T DualFenwickTree::get(int p) const;
```

$a_p$ を返します。

### 制約

- $0 \leq p < n$

### 計算量

- $O(\log n)$

## size

```cpp
int DualFenwickTree::size() const;
```

数列の長さ $n$ を返します。

### 計算量

- $O(1)$

## 使用例

```cpp
#include <iostream>
#include <vector>

#include "library3372/data_structure/dual_fenwick_tree.hpp"

int main() {
  std::vector<long long> a = {3, 1, 4, 1, 5};
  library3372::DualFenwickTree<long long> fw(a);

  fw.add(1, 4, 2);
  std::cout << fw.get(2) << "\n";  // 6
  std::cout << fw.get(4) << "\n";  // 5

  fw.set(0, 10);
  std::cout << fw.get(0) << "\n";  // 10
  return 0;
}
```
