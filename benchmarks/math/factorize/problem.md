# factorize

## 問題文

長さ $Q$ の正整数列 $A = (A_0, A_1, \dots, A_{Q - 1})$ が与えられます。各 $i = 0, 1, \dots, Q - 1$ について、 $A_i$ を素因数分解した結果 $A_i = \prod_{j = 0}^{k_i - 1} p_{i, j}$ ($p_{i, 0} \leq p_{i, 1} \leq \dots \leq p_{i, k_i - 1}$) を求めてください。

## 制約

- $1 \leq Q \leq 10^4$
- $1 \leq A_i \leq 2^{63} - 1$
- 入力はすべて整数

## 入力

入力は以下の形式で標準入力から与えられます。

```plaintext
Q
A_0 A_1 ... A_{Q-1}
```

## 出力

初期値 $h = 0$ から始め、 $i = 0, 1, \dots, Q - 1$ の順に `library3372/internal/hash/hash_combine.hpp` の `hash_combine` を用いて、 $k_i, p_{i, 0}, p_{i, 1}, \dots, p_{i, k_i - 1}$ を順に結合して得られる 64 bit 符号なし整数を出力してください。
