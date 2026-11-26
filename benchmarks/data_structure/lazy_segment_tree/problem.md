# lazy_segment_tree

## 問題文

長さ $N$ の整数列 $A = (A_0, A_1, \dots, A_{N-1})$ が与えられます。以下の $Q$ 個のクエリを順に処理してください。

- `0 l r b c`: $l \leq i < r$ を満たす各 $i$ について $A_i$ を $(b A_i + c) \bmod 998244353$ に変更する。
- `1 p x`: $A_p$ を $x$ に変更する。
- `2 p`: $A_p$ の値を記録する。
- `3 l r`: $(\sum_{i = l}^{r - 1} A_i) \bmod 998244353$ を記録する。ただし $l = r$ のときは $0$ とする。

## 制約

- $1 \leq N \leq 10^6$
- $1 \leq Q \leq 2 \times 10^6$
- $0 \leq A_i < 998244353$
- $0 \leq p < N$
- $0 \leq l \leq r \leq N$
- $0 \leq b, c, x < 998244353$

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
2. 各出力クエリ `2`, `3` の記録結果を順に `ans_hash = hash_combine(ans_hash, (unsigned long long)res)` として結合する。
