# convolution_large

## 問題文

長さ $N$ の整数列 $A = (A_0, A_1, \dots, A_{N - 1})$ と長さ $M$ の整数列 $B = (B_0, B_1, \dots, B_{M - 1})$ が与えられます。法を $P = 998244353$ とします。

各 $k = 0, 1, \dots, N + M - 2$ に対して、以下の値 $C_k$ を求めてください。

$$C_k = \left(\sum_{i + j = k} A_i B_j\right) \bmod P$$

## 制約

- $1 \leq N, M \leq 2^{24}$
- $0 \leq A_i, B_j < 998244353$
- 入力はすべて整数

## 入力

入力は以下の形式で標準入力から与えられます。

```plaintext
N M
A_0 A_1 ... A_{N-1}
B_0 B_1 ... B_{M-1}
```

## 出力

初期値 $h = 0$ から始め、 $k = 0, 1, \dots, N + M - 2$ の順に `library3372/internal/hash/hash_combine.hpp` の `hash_combine(h, C_k)` を適用して得られる 64 bit 符号なし整数を出力してください。
