# scc_graph

## 問題文

$N$ 頂点 $M$ 辺の有向グラフが与えられます。このグラフを強連結成分分解し、縮約後の有向非巡回グラフにおけるトポロジカルソート順で各強連結成分に番号 $0, 1, \dots, K - 1$ を割り当ててください。ただし、$K$ は強連結成分の個数です。

## 制約

- $1 \leq N \leq 10^6$
- $0 \leq M \leq 2 \times 10^6$
- $0 \leq u_i, v_i < N$

## 入力

```plaintext
N M
u_1 v_1
u_2 v_2
...
u_M v_M
```

## 出力

`library3372/internal/hash/hash_combine.hpp` の `hash_combine` を用いて、以下の手順で計算した 64 bit 符号なし整数値 `ans_hash` を出力する。

1. `ans_hash = hash_combine(0ULL, (unsigned long long)K)` と初期化する。
2. 各強連結成分について頂点番号を昇順に並べた配列 `g` を考え、`gh = (unsigned long long)g.size()` から始めて各頂点 `v` を順に `gh = hash_combine(gh, (unsigned long long)v)` で結合する。
3. すべての強連結成分についての `gh` の総和（$\bmod\ 2^{64}$）を `groups_hash` とする。
4. `ans_hash = hash_combine(ans_hash, groups_hash)` とする。
