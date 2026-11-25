# mod_mul

## 問題文

$Q$ 個のクエリが与えられます。 $i$ 番目 ($0 \leq i < Q$) のクエリでは奇数の法 $M_i$、初期値 $X_i, Y_i$、反復回数 $K_i$ が与えられます。

$a_0 = X_i$、 $b_0 = Y_i$ とおき、 $j = 0, 1, \dots, K_i - 1$ の順に以下を繰り返して $a_{K_i}, b_{K_i}$ を求めてください。

$$a_{j + 1} = (a_j \cdot b_j) \bmod M_i$$
$$b_{j + 1} = (b_j \cdot a_{j + 1}) \bmod M_i$$

## 制約

- $1 \leq Q \leq 10^6$
- $3 \leq M_i < 2^{63}$
- $M_i$ は奇数
- $0 \leq X_i, Y_i < M_i$
- $1 \leq K_i \leq 10^8$
- $\sum_{i = 0}^{Q - 1} K_i \leq 10^8$
- 入力はすべて整数

## 入力

入力は以下の形式で標準入力から与えられます。

```plaintext
Q
M_0 X_0 Y_0 K_0
M_1 X_1 Y_1 K_1
...
M_{Q-1} X_{Q-1} Y_{Q-1} K_{Q-1}
```

## 出力

初期値 $h = 0$ から始め、 $i = 0, 1, \dots, Q - 1$ の順に `library3372/internal/hash/hash_combine.hpp` の `hash_combine` を用いて $a_{K_i}$ と $b_{K_i}$ の順に結合して得られる 64 bit 符号なし整数を出力してください。
