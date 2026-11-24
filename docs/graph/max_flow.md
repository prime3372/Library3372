# Max Flow

有向グラフの最大フローと最小カットを求めるクラスです。頂点数と辺数を $n, m$ として、最大フローと最小カットの計算を $O(n^2 \sqrt{m})$ で行えます。すべての辺の容量が $1$ 以下の場合は $O((n + m)\sqrt{m})$ まで軽くなります。

## テンプレート引数

```cpp
template <typename Cap>
class MaxFlow;
```

辺の容量および流量を表す型 `Cap` を指定します。

### 制約

- `Cap` は `bool` を除く cv 修飾のない整数型

## メンバ型

### Edge

```cpp
struct Edge {
  int from;
  int to;
  Cap cap;
  Cap flow;
};
```

グラフに追加された有向辺の情報を表す構造体です。

- `from`: 始点
- `to`: 終点
- `cap`: 辺の容量
- `flow`: 現在の流量

## コンストラクタ

```cpp
MaxFlow::MaxFlow();       // (1)
MaxFlow::MaxFlow(int n);  // (2)
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
int MaxFlow::add_edge(int from, int to, Cap cap);
```

頂点 `from` から頂点 `to` へ容量 `cap`、初期流量 $0$ の有向辺を追加し、その辺の番号を返します。辺の番号は追加順に $0$ から割り当てられます。

### 制約

- $0 \leq \mathrm{from} < n$
- $0 \leq \mathrm{to} < n$
- $\mathrm{cap} \geq 0$

### 計算量

- 償却 $O(1)$
- `flow` 呼び出し後に初めて `add_edge` を呼び出す場合のみ $O(m)$

## get_edge

```cpp
Edge MaxFlow::get_edge(int i) const;
```

番号 $i$ の辺の現在の状態を返します。

### 制約

- $0 \leq i < m$

### 計算量

- $O(1)$

## edges

```cpp
std::vector<Edge> MaxFlow::edges() const;
```

追加されたすべての辺の現在の状態を番号順に並べた配列を返します。

### 計算量

- $O(m)$

## flow

```cpp
Cap MaxFlow::flow(int s, int t);                  // (1)
Cap MaxFlow::flow(int s, int t, Cap flow_limit);  // (2)
```

現在の残余グラフにおいて頂点 $s$ から頂点 $t$ へフローを流し、この呼び出しによって新たに流せた流量を返します。

- (1): 流せるだけフローを流します。
- (2): 合計流量が `flow_limit` 以下となる範囲で可能な限りフローを流します。

### 制約

- $0 \leq s < n$
- $0 \leq t < n$
- $s \neq t$
- (1): 新たに流せる最大流量が `Cap` の最大値以下
- (2): $\mathrm{flow\_limit} \geq 0$

### 計算量

- $O(n^2 \sqrt{m})$
- すべての辺の容量が $1$ 以下のとき、
  - $O((n + m)\sqrt{m})$
  - 多重辺がないとき $O((n + m)\min(m^{1/2}, n^{2/3}))$
  - $s, t$ 以外の各頂点について、残余グラフ上の入次数または出次数が $1$ 以下のとき $O((n + m)\sqrt{n})$

## min_cut

```cpp
std::vector<bool> MaxFlow::min_cut(int s) const;
```

現在の残余グラフ上で頂点 $s$ から到達可能な頂点を `true`、そうでない頂点を `false` とした長さ $n$ の真偽値配列を返します。`flow(s, t)` を呼び出した直後に呼ぶと、最小 $s$ - $t$ カットが得られます。

### 制約

- $0 \leq s < n$

### 計算量

- $O(n + m)$

## 使用例

```cpp
#include <iostream>

#include "library3372/graph/max_flow.hpp"

int main() {
  library3372::MaxFlow<int> mf(4);
  mf.add_edge(0, 1, 3);
  mf.add_edge(0, 2, 2);
  mf.add_edge(1, 2, 1);
  mf.add_edge(1, 3, 2);
  mf.add_edge(2, 3, 3);

  std::cout << mf.flow(0, 3) << "\n";  // 5

  for (int i = 0; i < 5; ++i) {
    auto e = mf.get_edge(i);
    std::cout << e.from << " -> " << e.to << " : " << e.flow << "/" << e.cap
              << "\n";
  }
  // 0 -> 1 : 3/3
  // 0 -> 2 : 2/2
  // 1 -> 2 : 1/1
  // 1 -> 3 : 2/2
  // 2 -> 3 : 3/3

  auto cut = mf.min_cut(0);
  for (int v = 0; v < 4; ++v) std::cout << cut[v] << (v == 3 ? "\n" : " ");
  // 1 0 0 0
  return 0;
}
```
