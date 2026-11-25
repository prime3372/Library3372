# sqrt_mod

## 問題文

長さ $Q$ の整数の組の列 $((A_0, P_0), (A_1, P_1), \dots, (A_{Q - 1}, P_{Q - 1}))$ が与えられます。各 $i = 0, 1, \dots, Q - 1$ について、 $x^2 \equiv A_i \pmod{P_i}$ を満たす最小の非負整数 $x$ を求めてください。

## 制約

- $1 \leq Q \leq 2 \times 10^5$
- $0 \leq A_i < P_i$
- $2 \leq P_i \leq 2^{63} - 1$
- $P_i$ は素数
- 入力はすべて整数

## 入力

入力は以下の形式で標準入力から与えられます。

```plaintext
Q
A_0 P_0
A_1 P_1
...
A_{Q-1} P_{Q-1}
```

## 出力

解が存在する場合は最小の非負整数解 $x_i$ を、存在しない場合は $2^{64} - 1$ を値 $v_i$ とします。初期値 $h = 0$ から始め、 $i = 0, 1, \dots, Q - 1$ の順に `library3372/internal/hash/hash_combine.hpp` の `hash_combine(h, v_i)` を適用して得られる 64 bit 符号なし整数を出力してください。
