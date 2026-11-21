# static_modint_raw

## 問題文

整数 $N, K$ と長さ $N$ の非負整数列 $A = (A_0, A_1, \dots, A_{N - 1})$ が与えられます。法を $M = 998244353$ とします。

長さ $N$ の整数列 $C = (C_0, C_1, \dots, C_{N - 1})$ をすべて $0$ で初期化します。$r = 0, 1, \dots, K - 1$ の順に、すべての $i = 0, 1, \dots, N - 1$ に対して以下の更新を同時に行います。

$$C_i \leftarrow (C_i + A_{(i + r) \bmod N}) \bmod M$$

最終的な数列 $C$ を求めてください。

## 制約

- $1 \leq N \leq 10^6$
- $1 \leq K \leq 10^6$
- $0 \leq A_i < 998244353$
- 入力はすべて整数

## 入力

入力は以下の形式で標準入力から与えられます。

```plaintext
N K
A_0 A_1 ... A_{N-1}
```

## 出力

初期値 $h = 0$ から始め、$i = 0, 1, \dots, N - 1$ の順に `library3372/internal/hash/hash_combine.hpp` の `hash_combine(h, C_i)` を適用して得られる 64 bit 符号なし整数を出力してください。
