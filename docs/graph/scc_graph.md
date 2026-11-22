# SCC Graph

有向グラフの強連結成分分解を計算量 $O(n + m)$ で行うクラスです。

## コンストラクタ

```cpp
SccGraph::SccGraph();       // (1)
SccGraph::SccGraph(int n);  // (2)
```

- (1): $0$ 頂点 $0$ 辺の有向グラフで初期化します。
- (2): $n$ 頂点 $0$ 辺の有向グラフで初期化します。

### 制約

- (2): $n \geq 0$

### 計算量

- (1): $O(1)$
- (2): $O(n)$

## add_edge

```cpp
void SccGraph::add_edge(int from, int to);
```

頂点 `from` から頂点 `to` への有向辺を追加します。

### 制約

- $0 \leq \mathrm{from} < n$
- $0 \leq \mathrm{to} < n$

### 計算量

- 償却 $O(1)$

## build

```cpp
void SccGraph::build();
```

強連結成分分解を行い、`count_groups()`、`ids()`、`groups()` で取得できる結果をすべて構築します。

### 計算量

- $O(n + m)$

## build_ids

```cpp
void SccGraph::build_ids();
```

強連結成分分解を行い、`count_groups()` および `ids()` で取得できる結果のみを構築します。`groups()` の結果は構築されません。

### 計算量

- $O(n + m)$

## count_groups

```cpp
int SccGraph::count_groups() const;
```

強連結成分の個数を返します。

### 制約

- `build()` または `build_ids()` を 1 回以上呼んでいる
- 直前に `build()` または `build_ids()` を呼んでから `add_edge()` を 1 回も呼んでいない

### 計算量

- $O(1)$

## ids

```cpp
const std::vector<int>& SccGraph::ids() const;
```

各頂点が属する強連結成分の番号を格納した配列 $\mathrm{ids}$ を返します。これは縮約 DAG におけるトポロジカルソート順になっています。すなわち以下が成り立ちます。

- 強連結成分の個数を $k$ として $0 \leq \mathrm{ids}[v] < k$
- 頂点 $u$ から頂点 $v$ への有向辺が存在する $\implies \mathrm{ids}[u] \leq \mathrm{ids}[v]$

### 制約

- `build()` または `build_ids()` を 1 回以上呼んでいる
- 直前に `build()` または `build_ids()` を呼んでから `add_edge()` を 1 回も呼んでいない

### 計算量

- $O(1)$

## groups

```cpp
const std::vector<std::vector<int>>& SccGraph::groups() const;
```

すべての強連結成分について「その成分に属する頂点番号の配列」をまとめた配列を返します。配列の $k$ 番目の要素には `ids()` で番号 $k$ が割り当てられた頂点が昇順に格納されます。

### 制約

- `build()` を 1 回以上呼んでいる
- 直前に `build()` を呼んでから `add_edge()` を 1 回も呼んでいない

### 計算量

- $O(1)$

## 補足

`build_ids()` を呼び出した後に `build()` を呼び出した場合は、`ids()` の計算結果を再利用して `groups()` 用の配列のみを $O(n)$ で構築します。

`add_edge()`、`build()`、`build_ids()` を呼び出すと、以前に取得した `ids()` や `groups()` の参照は無効になります。

## 使用例

```cpp
#include <iostream>

#include "library3372/graph/scc_graph.hpp"

int main() {
  library3372::SccGraph g(4);
  g.add_edge(0, 1);
  g.add_edge(1, 0);
  g.add_edge(1, 2);
  g.add_edge(2, 3);
  g.build();

  std::cout << g.count_groups() << "\n";  // 3

  const auto& id = g.ids();
  std::cout << (id[0] == id[1]) << "\n";  // 1
  std::cout << (id[0] < id[2]) << "\n";   // 1
  std::cout << (id[2] < id[3]) << "\n";   // 1

  for (const auto& comp : g.groups()) {
    for (int v : comp) std::cout << v << " ";
    std::cout << "\n";
  }
  // 0 1
  // 2
  // 3
  return 0;
}
```
