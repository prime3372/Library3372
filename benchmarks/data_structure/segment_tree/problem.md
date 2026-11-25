# segment_tree

## 問題文

長さ $N$ の非負整数列 $A = (A_0, A_1, \dots, A_{N-1})$ が与えられます。以下の $Q$ 個のクエリを順に処理してください。

- `0 p x`: $A_p$ を $x$ に変更する。
- `1 p`: $A_p$ の値を記録する。
- `2 l r`: $\max_{l \leq i < r} A_i$ を記録する。ただし $l = r$ のときは $0$ とする。
- `3 l x`: $\max_{l \leq i < r} A_i \leq x$ を満たす最大の $r$ ($l \leq r \leq N$) を記録する。ただし $l = r$ のときの左辺は $0$ とみなす。
- `4 r x`: $\max_{l \leq i < r} A_i \leq x$ を満たす最小の $l$ ($0 \leq l \leq r$) を記録する。ただし $l = r$ のときの左辺は $0$ とみなす。

## 制約

- $1 \leq N \leq 10^6$
- $1 \leq Q \leq 2 \times 10^6$
- $0 \leq A_i \leq 10^9$
- $0 \leq p < N$
- $0 \leq l \leq r \leq N$
- $0 \leq x \leq 10^9$

## 入力

```plaintext
N Q
A_0 A_1 ... A_{N-1}
query_0
query_1
...
query_{Q-1}
```

## 出力

`library3372/internal/hash/hash_combine.hpp` の `hash_combine` を用いて、以下の手順で計算した 64 bit 符号なし整数値 `ans_hash` を出力する。

1. `ans_hash = 0` と初期化する。
2. 各出力クエリ `1`, `2`, `3`, `4` の記録結果を順に `ans_hash = hash_combine(ans_hash, (unsigned long long)res)` として結合する。
