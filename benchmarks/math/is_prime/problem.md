# is_prime

## 問題文

長さ $Q$ の整数列 $A = (A_0, A_1, \dots, A_{Q - 1})$ が与えられます。各 $i = 0, 1, \dots, Q - 1$ について、 $A_i$ が素数であれば $R_i = 1$、そうでなければ $R_i = 0$ として $R_i$ を求めてください。

## 制約

- $1 \leq Q \leq 2 \times 10^6$
- $0 \leq A_i \leq 2^{63} - 1$
- 入力はすべて整数

## 入力

入力は以下の形式で標準入力から与えられます。

```plaintext
Q
A_0 A_1 ... A_{Q-1}
```

## 出力

初期値 $h = 0$ から始め、 $i = 0, 1, \dots, Q - 1$ の順に `library3372/internal/hash/hash_combine.hpp` の `hash_combine(h, R_i)` を適用して得られる 64 bit 符号なし整数を出力してください。
