# primitive_root

## 問題文

長さ $Q$ の素数列 $P = (P_0, P_1, \dots, P_{Q - 1})$ が与えられます。各 $i = 0, 1, \dots, Q - 1$ について、 $P_i$ に対する最小の原始根 $g_i$ を求めてください。

## 制約

- $1 \leq Q \leq 10^4$
- $2 \leq P_i \leq 2^{63} - 1$
- $P_i$ は素数
- 入力はすべて整数

## 入力

入力は以下の形式で標準入力から与えられます。

```plaintext
Q
P_0 P_1 ... P_{Q-1}
```

## 出力

初期値 $h = 0$ から始め、 $i = 0, 1, \dots, Q - 1$ の順に `library3372/internal/hash/hash_combine.hpp` の `hash_combine(h, g_i)` を適用して得られる 64 bit 符号なし整数を出力してください。
