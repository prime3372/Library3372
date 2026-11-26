# Lazy Segment Tree

長さ $n$ の数列 $a_0, a_1, \dots, a_{n-1}$ に対する一点更新・区間モノイド積取得・区間作用を構築 $O(n)$ クエリあたり $O(\log n)$ で処理します。

## テンプレート引数

```cpp
template <typename S, auto op, auto e, typename F, auto mapping,
          auto composition, auto id>
class LazySegmentTree;
```

以下を指定します。

- モノイド $(S, \mathrm{op}, e)$
- 作用素の集合 $F$
- 写像 $\mathrm{mapping} : F \times S \to S$
- 作用素の合成 $\mathrm{composition} : F \times F \to F$
- 恒等作用素 $\mathrm{id} \in F$

簡略化のため $\mathrm{op}(x, y)$ を $xy$、 $\mathrm{mapping}(f, x)$ を $f(x)$、 $\mathrm{composition}(g, f)$ を $g \circ f$ と表記します。

### 制約

- $(S, \mathrm{op}, e)$ はモノイドをなす
- $(F, \mathrm{composition}, \mathrm{id})$ はモノイドをなす
- 任意の $f \in F$ と $x, y \in S$ について $f(xy) = f(x)f(y)$ が成り立つ
- 任意の $f, g \in F$ と $x \in S$ について $(g \circ f)(x) = g(f(x))$ が成り立つ
- 任意の $x \in S$ について $\mathrm{id}(x) = x$ が成り立つ
- `S` と `F` は cv 修飾のないコピー可能なオブジェクト型
- `op` は `op(const S&, const S&)` の形で呼び出し可能であり、`S` に変換可能な値を返す
- `e` は `e()` の形で呼び出し可能であり、`S` に変換可能な値を返す
- `mapping` は `mapping(const F&, const S&)` の形で呼び出し可能であり、`S` に変換可能な値を返す
- `composition` は `composition(const F&, const F&)` の形で呼び出し可能であり、`F` に変換可能な値を返す
- `id` は `id()` の形で呼び出し可能であり、`F` に変換可能な値を返す

### 補足

$(S, \mathrm{op}, e, F, \mathrm{mapping}, \mathrm{composition}, \mathrm{id})$ が厳密な作用付きモノイドをなさない場合でも、以下の条件をすべて満たすときは正しく動作します。

- 任意の時点における任意の連続部分列 $a_l, a_{l+1}, \dots, a_{r-1}$ について $a_l a_{l+1} \cdots a_{r-1} \in S$
- 任意の要素に対し、その要素に適用された作用素を順に $f_0, f_1, \dots, f_{k-1}$ としたとき、任意の $0 \leq l < r \leq k$ について $f_{r-1} \circ \dots \circ f_{l+1} \circ f_l \in F$
- $e \in S$
- 任意の $x \in S$ について $ex = xe = x$
- 任意の $x, y, z \in S$ について $xy, yz, (xy)z, x(yz) \in S$ ならば $(xy)z = x(yz)$
- $\mathrm{id} \in F$
- 任意の $f \in F$ について $\mathrm{id} \circ f = f \circ \mathrm{id} = f$
- 任意の $f, g, h \in F$ について $f \circ g, g \circ h, (f \circ g) \circ h, f \circ (g \circ h) \in F$ ならば $(f \circ g) \circ h = f \circ (g \circ h)$
- 任意の $f \in F$ と $x, y \in S$ について $xy, f(x), f(y), f(xy), f(x)f(y) \in S$ ならば $f(xy) = f(x)f(y)$
- 任意の $f, g \in F$ と $x \in S$ について $g \circ f \in F$ かつ $f(x), g(f(x)), (g \circ f)(x) \in S$ ならば $(g \circ f)(x) = g(f(x))$
- 任意の $x \in S$ について $\mathrm{id}(x) = x$

## コンストラクタ

```cpp
LazySegmentTree::LazySegmentTree();                         // (1)
LazySegmentTree::LazySegmentTree(int n);                    // (2)
LazySegmentTree::LazySegmentTree(const std::vector<S>& a);  // (3)
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
void LazySegmentTree::set(int p, const S& x);
```

$a_p$ の値を $x$ に変更します。

### 制約

- $0 \leq p < n$

### 計算量

- $O(\log n)$

## get

```cpp
S LazySegmentTree::get(int p);
```

$a_p$ を返します。

### 制約

- $0 \leq p < n$

### 計算量

- $O(\log n)$

## prod

```cpp
S LazySegmentTree::prod(int l, int r);
```

$a_l a_{l+1} \cdots a_{r-1}$ を返します。 $l = r$ のときは `e()` を返します。

### 制約

- $0 \leq l \leq r \leq n$

### 計算量

- $O(\log n)$

## all_prod

```cpp
S LazySegmentTree::all_prod() const;
```

$a_0a_1 \cdots a_{n-1}$ を返します。 $n = 0$ のときは `e()` を返します。

### 計算量

- $O(1)$

## apply

```cpp
void LazySegmentTree::apply(int p, const F& f);         // (1)
void LazySegmentTree::apply(int l, int r, const F& f);  // (2)
```

- (1): $a_p$ を $f(a_p)$ に変更します。
- (2): すべての $l \leq i < r$ に対して $a_i$ を $f(a_i)$ に変更します。

### 制約

- (1): $0 \leq p < n$
- (2): $0 \leq l \leq r \leq n$

### 計算量

- $O(\log n)$

## max_right

```cpp
template <typename Pred>
int LazySegmentTree::max_right(int l, Pred pred);
```

以下の条件をすべて満たす整数 $r$ を返します。

- $l \leq r \leq n$
- $l < i \leq r$ に対して $\mathrm{pred}(a_l a_{l+1} \cdots a_{i-1}) = \mathrm{true}$
- $r < i \leq n$ に対して $\mathrm{pred}(a_l a_{l+1} \cdots a_{i-1}) = \mathrm{false}$

### 制約

- `pred` は `pred(const S&)` の形で呼び出し可能であり、`bool` を返す述語である
- $0 \leq l \leq n$
- $\mathrm{pred}(e) = \mathrm{true}$
- 条件を満たす $r$ が存在する

### 計算量

- $O(\log n)$

## min_left

```cpp
template <typename Pred>
int LazySegmentTree::min_left(int r, Pred pred);
```

以下の条件をすべて満たす整数 $l$ を返します。

- $0 \leq l \leq r$
- $l \leq i < r$ に対して $\mathrm{pred}(a_i a_{i+1} \cdots a_{r-1}) = \mathrm{true}$
- $0 \leq i < l$ に対して $\mathrm{pred}(a_i a_{i+1} \cdots a_{r-1}) = \mathrm{false}$

### 制約

- `pred` は `pred(const S&)` の形で呼び出し可能であり、`bool` を返す述語である
- $0 \leq r \leq n$
- $\mathrm{pred}(e) = \mathrm{true}$
- 条件を満たす $l$ が存在する

### 計算量

- $O(\log n)$

## size

```cpp
int LazySegmentTree::size() const;
```

数列の長さ $n$ を返します。

### 計算量

- $O(1)$

## 使用例

```cpp
#include <algorithm>
#include <iostream>
#include <vector>

#include "library3372/data_structure/lazy_segment_tree.hpp"

int main() {
  constexpr auto op = [](long long a, long long b) { return std::max(a, b); };
  constexpr auto e = []() { return -1LL; };
  constexpr auto mapping = [](long long f, long long x) { return f + x; };
  constexpr auto composition = [](long long f, long long g) { return f + g; };
  constexpr auto id = []() { return 0LL; };

  std::vector<long long> init = {1, 2, 3, 4, 5};
  library3372::LazySegmentTree<long long, op, e, long long, mapping,
                               composition, id>
      seg(init);

  seg.apply(1, 4, 3);
  std::cout << seg.prod(0, 5) << "\n";  // 7
  std::cout << seg.get(2) << "\n";      // 6
  std::cout << seg.prod(2, 2) << "\n";  // -1

  int r = seg.max_right(0, [](long long x) { return x < 6; });
  std::cout << r << "\n";  // 2
  return 0;
}
```
