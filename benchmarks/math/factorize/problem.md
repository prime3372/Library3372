# factorize

## 問題文

長さ $Q$ の正整数列 $A = (A_1, A_2, \dots, A_Q)$ が与えられます。各 $i = 1, 2, \dots, Q$ について、 $A_i$ を素因数分解した結果 $A_i = \prod_{j=1}^{k_i} p_{i,j}$ ($p_{i,1} \leq p_{i,2} \leq \dots \leq p_{i,k_i}$) を求めてください。

## 制約

- $1 \leq Q \leq 10^4$
- $1 \leq A_i \leq 2^{63} - 1$
- 入力はすべて整数

## 入力

入力は以下の形式で標準入力から与えられます。

```plaintext
Q
A_1 A_2 ... A_Q
```

## 出力

初期値 $h = 0$ から始め、 $i = 1, 2, \dots, Q$ の順に `library3372/internal/hash/hash_combine.hpp` の `hash_combine` を用いて、 $k_i, p_{i,1}, p_{i,2}, \dots, p_{i,k_i}$ を順に結合して得られる 64 bit 符号なし整数を出力してください。
