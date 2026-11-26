# range_add_range_sum

## 問題文

長さ $N$ の整数列 $A = (A_0, A_1, \dots, A_{N-1})$ が与えられます。以下の $Q$ 個のクエリを順に処理してください。

- `0 l r x`: $l \leq i < r$ を満たす各 $i$ について $A_i$ に $x$ を加算する。
- `1 l r`: $\sum_{i = l}^{r - 1} A_i$ を記録する。ただし $l = r$ のときは $0$ とする。
- `2 l x`: $\sum_{i = l}^{r - 1} A_i \leq x$ を満たす最大の $r$ ($l \leq r \leq N$) を記録する。ただし、このクエリが与えられる時点では $A_i \geq 0$ ($0 \leq i < N$) が成り立つ。
- `3 r x`: $\sum_{i = l}^{r - 1} A_i \leq x$ を満たす最小の $l$ ($0 \leq l \leq r$) を記録する。ただし、このクエリが与えられる時点では $A_i \geq 0$ ($0 \leq i < N$) が成り立つ。

## 制約

- $1 \leq N \leq 10^6$
- $1 \leq Q \leq 2 \times 10^6$
- $-10^6 \leq A_i \leq 10^6$
- クエリ `0` では $-10^6 \leq x \leq 10^6$
- $0 \leq l \leq r \leq N$
- 初期値と $x$ およびクエリ処理中の総和は 64 bit 符号付き整数型に収まる
- クエリ `2`, `3` が与えられる時点では $A_i \geq 0$ ($0 \leq i < N$) かつ $x \geq 0$

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
2. 各出力クエリ `1`, `2`, `3` の記録結果を順に `ans_hash = hash_combine(ans_hash, (unsigned long long)res)` として結合する。
