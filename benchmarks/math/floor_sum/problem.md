# floor_sum

## 問題文

長さ $Q$ の整数の $4$ つ組の列 $((N_0, M_0, A_0, B_0), \dots, (N_{Q - 1}, M_{Q - 1}, A_{Q - 1}, B_{Q - 1}))$ が与えられます。各 $i = 0, 1, \dots, Q - 1$ について、 $R_i = \left(\sum_{k = 0}^{N_i - 1} \lfloor (A_i k + B_i) / M_i \rfloor\right) \bmod 2^{64}$ を求めてください。

## 制約

- $1 \leq Q \leq 10^6$
- $0 \leq N_i \leq 2^{63} - 1$
- $1 \leq M_i \leq 2^{63} - 1$
- $-2^{63} \leq A_i, B_i \leq 2^{63} - 1$
- 入力はすべて整数

## 入力

入力は以下の形式で標準入力から与えられます。

```plaintext
Q
N_0 M_0 A_0 B_0
N_1 M_1 A_1 B_1
...
N_{Q-1} M_{Q-1} A_{Q-1} B_{Q-1}
```

## 出力

初期値 $h = 0$ から始め、 $i = 0, 1, \dots, Q - 1$ の順に `library3372/internal/hash/hash_combine.hpp` の `hash_combine(h, R_i)` を適用して得られる 64 bit 符号なし整数を出力してください。
