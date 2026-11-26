# dual_fenwick_tree

## 問題文

長さ $N$ の整数列 $A = (A_0, A_1, \dots, A_{N-1})$ が与えられます。以下の $Q$ 個のクエリを順に処理してください。

- `0 l r x`: $l \leq i < r$ を満たす各 $i$ について $A_i$ に $x$ を加算する。
- `1 p x`: $A_p$ を $x$ に変更する。
- `2 p`: $A_p$ の値を記録する。

## 制約

- $1 \leq N \leq 10^6$
- $1 \leq Q \leq 2 \times 10^6$
- $-10^9 \leq A_i, x \leq 10^9$
- $0 \leq p < N$
- $0 \leq l \leq r \leq N$
- 初期値およびクエリ処理中の各要素は 64 bit 符号付き整数型に収まる

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
2. 各出力クエリ `2` の記録結果を順に `ans_hash = hash_combine(ans_hash, (unsigned long long)res)` として結合する。
