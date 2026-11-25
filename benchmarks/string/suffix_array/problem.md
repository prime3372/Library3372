# suffix_array

## 問題文

長さ $N$ の非負整数列 $A = (A_0, A_1, \dots, A_{N - 1})$ が与えられます。この列の接尾辞配列 $\mathrm{sa} = (\mathrm{sa}_0, \mathrm{sa}_1, \dots, \mathrm{sa}_{N - 1})$ を求めてください。

すなわち、 $0, 1, \dots, N - 1$ の順列であって、すべての $0 \leq i < N - 1$ について $(A_{\mathrm{sa}_i}, A_{\mathrm{sa}_i + 1}, \dots, A_{N - 1}) < (A_{\mathrm{sa}_{i + 1}}, A_{\mathrm{sa}_{i + 1} + 1}, \dots, A_{N - 1})$ を辞書順で満たすものを求めてください。

## 制約

- $1 \leq N \leq 2 \times 10^6$
- $0 \leq A_i \leq 10^9$
- 入力はすべて整数

## 入力

```plaintext
N
A_0 A_1 ... A_{N-1}
```

## 出力

`library3372/internal/hash/hash_combine.hpp` の `hash_combine` を用いて、以下の手順で計算した 64 bit 符号なし整数値 `ans_hash` を出力する。

1. `ans_hash = 0ULL` と初期化する。
2. $i = 0, 1, \dots, N - 1$ の順に `ans_hash = hash_combine(ans_hash, (unsigned long long)sa_i)` で結合する。
