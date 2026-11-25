# disjoint_sparse_table

## 問題文

長さ $N$ の一次関数列 $f_0, f_1, \dots, f_{N-1}$ があります。ここで $f_i(x) = (a_i x + b_i) \bmod 998244353$ です。

以下の $Q$ 個のクエリに答えてください。

- $l_j\ r_j$: 合成関数 $g(x) = (f_{r_j - 1} \circ \dots \circ f_{l_j + 1} \circ f_{l_j})(x) = (c_j x + d_j) \bmod 998244353$ の係数 $(c_j, d_j)$ を求める。ただし $l_j = r_j$ のときは $(1, 0)$ とする。

## 制約

- $1 \leq N \leq 10^6$
- $1 \leq Q \leq 2 \times 10^6$
- $0 \leq a_i, b_i < 998244353$
- $0 \leq l_j \leq r_j \leq N$

## 入力

```plaintext
N Q
a_0 b_0
a_1 b_1
...
a_{N-1} b_{N-1}
l_0 r_0
l_1 r_1
...
l_{Q-1} r_{Q-1}
```

## 出力

`library3372/internal/hash/hash_combine.hpp` の `hash_combine` を用いて、以下の手順で計算した 64 bit 符号なし整数値 `ans_hash` を出力する。

1. `ans_hash = 0` と初期化する。
2. 各クエリ $j = 0, 1, \dots, Q-1$ について、順に以下の通り結合する。
   - `ans_hash = hash_combine(ans_hash, (unsigned long long)c_j)`
   - `ans_hash = hash_combine(ans_hash, (unsigned long long)d_j)`
