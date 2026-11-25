# union_find

## 問題文

$N$ 頂点 $0$ 辺の無向グラフが与えられます。以下の $Q$ 個のクエリを順に処理し、最後にすべての連結成分を列挙してください。

- `0 u v`: 頂点 $u$ と頂点 $v$ の間に無向辺を追加する。元々異なる連結成分に属していた場合は、合併後の代表元と吸収された古い代表元を記録する。
- `1 u v`: 頂点 $u$ と頂点 $v$ が同じ連結成分に属していれば `1` を、そうでなければ `0` を記録する。
- `2 u`: 頂点 $u$ が属する連結成分の頂点数を記録する。
- `3 u`: 頂点 $u$ が属する連結成分の代表元を記録する。
- `4`: 現在の連結成分の個数を記録する。

初期状態では各頂点 $i$ ($0 \leq i < N$) がそれ自身の連結成分の代表元です。クエリ `0 u v` によって異なる連結成分が合併されるとき、新たな代表元は元の連結成分のうち頂点数が小さくない方の代表元とし、頂点数が等しい場合は $u$ が属していた連結成分の代表元とします。

## 制約

- $1 \leq N \leq 10^6$
- $1 \leq Q \leq 3 \times 10^6$
- $0 \leq u, v < N$

## 入力

```plaintext
N Q
query_1
query_2
...
query_Q
```

## 出力

`library3372/internal/hash/hash_combine.hpp` の `hash_combine` を用いて、以下の手順で計算した 64 bit 符号なし整数値 `ans_hash` を出力する。

1. `ans_hash = 0` と初期化する。
2. 各クエリの処理結果を順に `ans_hash` へ結合する。
   - `0 u v`: 合併が起きた場合は `ans_hash = hash_combine(ans_hash, new_root)`、続いて `ans_hash = hash_combine(ans_hash, old_root)` とする。合併が起きなかった場合は `ans_hash = hash_combine(ans_hash, 0ULL)` とする。
   - `1 u v`: 同じ連結成分であれば `1ULL`、そうでなければ `0ULL` を結合する。
   - `2 u`: 連結成分の頂点数を結合する。
   - `3 u`: 代表元を結合する。
   - `4`: 連結成分の個数を結合する。
3. 全クエリ処理後、各連結成分について頂点番号を昇順に並べた配列 `g` を考え、`gh = (unsigned long long)g.size()` から始めて各頂点 `v` を順に `gh = hash_combine(gh, (unsigned long long)v)` で結合する。すべての連結成分についての `gh` の総和 ($\bmod\ 2^{64}$) を `groups_hash` とする。
4. `ans_hash = hash_combine(ans_hash, groups_hash)` とする。
