# isqrt

## 問題文

長さ $Q$ の非負整数列 $A = (A_1, A_2, \dots, A_Q)$ が与えられます。各 $i = 1, 2, \dots, Q$ について、$R_i = \lfloor \sqrt{A_i} \rfloor$ を求めてください。

## 制約

- $1 \leq Q \leq 10^6$
- $0 \leq A_i \leq 2^{63} - 1$
- 入力はすべて整数

## 入力

入力は以下の形式で標準入力から与えられます。

```plaintext
Q
A_1 A_2 ... A_Q
```

## 出力

初期値 $h = 0$ から始め、$i = 1, 2, \dots, Q$ の順に `library3372/internal/hash/hash_combine.hpp` の `hash_combine(h, R_i)` を適用して得られる 64 bit 符号なし整数を出力してください。
