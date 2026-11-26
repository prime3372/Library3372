# Segment Tree

長さ $n$ の数列 $a_0, a_1, \dots, a_{n-1}$ に対する一点更新と区間モノイド積取得を構築 $O(n)$ クエリあたり $O(\log n)$ で処理します。

## テンプレート引数

```cpp
template <typename S, auto op, auto e>
class SegmentTree;
```

モノイド $(S, \mathrm{op}, e)$ を指定します。簡略化のため $\mathrm{op}(x, y)$ を $xy$ と表記します。

### 制約

- $(S, \mathrm{op}, e)$ はモノイドをなす
- `S` は cv 修飾のないコピー可能なオブジェクト型
- `op` は `op(const S&, const S&)` の形で呼び出し可能であり、`S` に変換可能な値を返す
- `e` は `e()` の形で呼び出し可能であり、`S` に変換可能な値を返す

### 補足

$(S, \mathrm{op}, e)$ が厳密なモノイドをなさない場合でも、以下の条件をすべて満たすときは正しく動作します。

- 任意の時点における任意の連続部分列 $a_l, a_{l+1}, \dots, a_{r-1}$ について $a_l a_{l+1} \cdots a_{r-1} \in S$
- $e \in S$
- 任意の $x \in S$ について $ex = xe = x$
- 任意の $x, y, z \in S$ について $xy, yz, (xy)z, x(yz) \in S$ ならば $(xy)z = x(yz)$

## コンストラクタ

```cpp
SegmentTree::SegmentTree();                         // (1)
SegmentTree::SegmentTree(int n);                    // (2)
SegmentTree::SegmentTree(const std::vector<S>& a);  // (3)
```

- (1): 長さ $0$ の数列で初期化します。
- (2): 長さ $n$ の数列をすべて単位元 `e()` で初期化します。
- (3): 配列 `a` を初期値として長さ $n$ の数列を構築します。

### 制約

- $0 \leq n \leq 2^{30}$

### 計算量

- (1): $O(1)$
- (2), (3): $O(n)$

## set

```cpp
void SegmentTree::set(int p, const S& x);
```

$a_p$ の値を $x$ に変更します。

### 制約

- $0 \leq p < n$

### 計算量

- $O(\log n)$

## get

```cpp
S SegmentTree::get(int p) const;
```

$a_p$ を返します。

### 制約

- $0 \leq p < n$

### 計算量

- $O(1)$

## prod

```cpp
S SegmentTree::prod(int l, int r) const;
```

$a_l a_{l+1} \cdots a_{r-1}$ を返します。 $l = r$ のときは `e()` を返します。

### 制約

- $0 \leq l \leq r \leq n$

### 計算量

- $O(\log n)$

## all_prod

```cpp
S SegmentTree::all_prod() const;
```

$a_0a_1 \cdots a_{n-1}$ を返します。 $n = 0$ のときは `e()` を返します。

### 計算量

- $O(1)$

## max_right

```cpp
template <typename F>
int SegmentTree::max_right(int l, F f) const;
```

以下の条件をすべて満たす整数 $r$ を返します。

- $l \leq r \leq n$
- $l < i \leq r$ に対して $f(a_l a_{l+1} \cdots a_{i-1}) = \mathrm{true}$
- $r < i \leq n$ に対して $f(a_l a_{l+1} \cdots a_{i-1}) = \mathrm{false}$

### 制約

- `f` は `f(const S&)` の形で呼び出し可能であり、`bool` を返す述語である
- $0 \leq l \leq n$
- $f(e) = \mathrm{true}$
- 条件を満たす $r$ が存在する

### 計算量

- $O(\log n)$

## min_left

```cpp
template <typename F>
int SegmentTree::min_left(int r, F f) const;
```

以下の条件をすべて満たす整数 $l$ を返します。

- $0 \leq l \leq r$
- $l \leq i < r$ に対して $f(a_i a_{i+1} \cdots a_{r-1}) = \mathrm{true}$
- $0 \leq i < l$ に対して $f(a_i a_{i+1} \cdots a_{r-1}) = \mathrm{false}$

### 制約

- `f` は `f(const S&)` の形で呼び出し可能であり、`bool` を返す述語である
- $0 \leq r \leq n$
- $f(e) = \mathrm{true}$
- 条件を満たす $l$ が存在する

### 計算量

- $O(\log n)$

## size

```cpp
int SegmentTree::size() const;
```

数列の長さ $n$ を返します。

### 計算量

- $O(1)$

## 使用例

```cpp
#include <algorithm>
#include <iostream>
#include <vector>

#include "library3372/data_structure/segment_tree.hpp"

int main() {
  constexpr auto op = [](long long a, long long b) { return std::max(a, b); };
  constexpr auto e = []() { return -1LL; };

  std::vector<long long> a = {3, 1, 4, 1, 5};
  library3372::SegmentTree<long long, op, e> seg(a);

  std::cout << seg.prod(1, 4) << "\n";  // 4
  seg.set(3, 6);
  std::cout << seg.prod(1, 4) << "\n";  // 6
  std::cout << seg.all_prod() << "\n";  // 6
  std::cout << seg.prod(2, 2) << "\n";  // -1

  int r = seg.max_right(0, [](long long x) { return x < 5; });
  std::cout << r << "\n";  // 3
  return 0;
}
```
