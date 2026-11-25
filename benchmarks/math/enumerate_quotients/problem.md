# enumerate_quotients

## 問題文

長さ $Q$ の非負整数列 $A = (A_0, A_1, \dots, A_{Q - 1})$ が与えられます。各 $i = 0, 1, \dots, Q - 1$ について、 $1 \leq x \leq A_i$ における $\lfloor A_i / x \rfloor$ のとり得るすべての値 $q$ と、 $\lfloor A_i / x \rfloor = q$ となる半開区間 $[l, r)$ の組 $(q, l, r)$ を $q$ の昇順ですべて求めてください。

## 制約

- $1 \leq Q \leq 10^5$
- $0 \leq A_i \leq 2^{63} - 2$
- 入力はすべて整数

## 入力

入力は以下の形式で標準入力から与えられます。

```plaintext
Q
A_0 A_1 ... A_{Q-1}
```

## 出力

初期値 $h = 0$ から始め、 $i = 0, 1, \dots, Q - 1$ の順に `library3372/internal/hash/hash_combine.hpp` の `hash_combine` を用いて、 $A_i$ に対する組の個数 $k_i$ を結合し、さらに各組 $(q, l, r)$ について $q, l, r$ を順に結合して得られる 64 bit 符号なし整数を出力してください。
