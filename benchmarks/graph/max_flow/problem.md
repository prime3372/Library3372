# max_flow

## 問題文

$N$ 頂点 $M$ 辺の容量付き有向グラフが与えられます。 $i$ 番目 ($1 \leq i \leq M$) の辺は頂点 $u_i$ から頂点 $v_i$ へ向かい、容量は $c_i$ です。

頂点 $S$ から頂点 $T$ への最大フロー値 $F$、および最小 $S$-$T$ カットの $S$ 側頂点集合（残余グラフにおいて $S$ から正容量の辺のみを辿って到達可能な頂点集合）を求めてください。

## 制約

- $2 \leq N \leq 2 \times 10^5$
- $1 \leq M \leq 5 \times 10^5$
- $0 \leq S, T < N$
- $S \neq T$
- $0 \leq u_i, v_i < N$
- $0 \leq c_i \leq 10^9$
- 入力はすべて整数

## 入力

```plaintext
N M S T
u_1 v_1 c_1
u_2 v_2 c_2
...
u_M v_M c_M
```

## 出力

`library3372/internal/hash/hash_combine.hpp` の `hash_combine` を用いて、以下の手順で計算した 64 bit 符号なし整数値 `ans_hash` を出力する。

1. `ans_hash = (unsigned long long)F` と初期化する。
2. 全辺の状態から求めた頂点 $S$ の正味流出量 `out_s`（頂点 $S$ から出る辺の流量の和から $S$ へ入る辺の流量の和を引いた値）を `ans_hash = hash_combine(ans_hash, (unsigned long long)out_s)` で結合する。
3. $v = 0, 1, \dots, N - 1$ の順に、頂点 $v$ が最小 $S$-$T$ カットの $S$ 側であれば `1ULL`、そうでなければ `0ULL` を `ans_hash = hash_combine(ans_hash, ...)` で結合する。
