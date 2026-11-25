# 動的 modint

実行時に決定可能な法 $m$ による剰余環 $\mathbb{Z}/m\mathbb{Z}$ の元を表すクラスです。環演算や比較を $O(1)$ で、べき乗や逆元の計算を $O(\log m)$ で行えます。

## テンプレート引数

```cpp
template <int id>
class DynamicModInt;
```

法を識別するための整数 `id` を指定します。同じ `id` を指定した `DynamicModInt` 型同士は同じ法を共有します。

## コンストラクタ

```cpp
DynamicModInt::DynamicModInt();  // (1)

template <typename T>
DynamicModInt::DynamicModInt(T v);  // (2)
```

- (1): 値 $0$ で初期化します。
- (2): 整数 $v$ を現在の法 $m$ で割った余りで初期化します。

### 制約

- (2): `T` は整数型

### 計算量

- $O(1)$

## mod

```cpp
static int DynamicModInt::mod();
```

現在の法 $m$ を返します。

### 計算量

- $O(1)$

## set_mod

```cpp
static void DynamicModInt::set_mod(int m);
```

法を $m$ に設定します。

### 制約

- $m \geq 1$

### 計算量

- $O(1)$

## raw

```cpp
template <typename T>
static DynamicModInt DynamicModInt::raw(T v);
```

剰余を取らずに値 $v$ で初期化した `DynamicModInt` を返します。

### 制約

- `T` は整数型
- $0 \leq v < m$

### 計算量

- $O(1)$

## val

```cpp
int DynamicModInt::val() const;
```

保持している値を $0$ 以上 $m$ 未満の整数として返します。

### 計算量

- $O(1)$

## pow

```cpp
DynamicModInt DynamicModInt::pow(long long n) const;
```

自身の $n$ 乗を返します。

### 制約

- $n \geq 0$

### 計算量

- $O(\log n)$

## inv

```cpp
DynamicModInt DynamicModInt::inv() const;
```

法 $m$ における乗法逆元を返します。

### 制約

- $\gcd(\mathrm{val}(), m) = 1$

### 計算量

- $O(\log m)$

## operator++

```cpp
DynamicModInt& DynamicModInt::operator++();    // (1)
DynamicModInt DynamicModInt::operator++(int);  // (2)
```

値に $1$ を加算します。

- (1): 前置インクリメントです。インクリメント後のオブジェクトの参照が返ります。
- (2): 後置インクリメントです。インクリメント前のオブジェクトのコピーが返ります。

### 計算量

- $O(1)$

## operator--

```cpp
DynamicModInt& DynamicModInt::operator--();    // (1)
DynamicModInt DynamicModInt::operator--(int);  // (2)
```

値から $1$ を減算します。

- (1): 前置デクリメントです。デクリメント後のオブジェクトの参照が返ります。
- (2): 後置デクリメントです。デクリメント前のオブジェクトのコピーが返ります。

### 計算量

- $O(1)$

## operator+=

```cpp
DynamicModInt& DynamicModInt::operator+=(DynamicModInt rhs);
```

`rhs` を加算します。

### 計算量

- $O(1)$

## operator-=

```cpp
DynamicModInt& DynamicModInt::operator-=(DynamicModInt rhs);
```

`rhs` を減算します。

### 計算量

- $O(1)$

## operator\*=

```cpp
DynamicModInt& DynamicModInt::operator*=(DynamicModInt rhs);
```

`rhs` を乗算します。

### 計算量

- $O(1)$

## operator/=

```cpp
DynamicModInt& DynamicModInt::operator/=(DynamicModInt rhs);
```

`rhs` で除算します。

### 制約

- $\gcd(\mathrm{rhs}.\mathrm{val}(), m) = 1$

### 計算量

- $O(\log m)$

## operator+

```cpp
DynamicModInt DynamicModInt::operator+() const;  // (1)

DynamicModInt operator+(DynamicModInt lhs, DynamicModInt rhs);  // (2)
```

- (1): 自身の値をそのまま返します。
- (2): `lhs` と `rhs` の和を返します。

### 計算量

- $O(1)$

## operator-

```cpp
DynamicModInt DynamicModInt::operator-() const;  // (1)

DynamicModInt operator-(DynamicModInt lhs, DynamicModInt rhs);  // (2)
```

- (1): 加法逆元を返します。
- (2): `lhs` と `rhs` の差を返します。

### 計算量

- $O(1)$

## operator\*

```cpp
DynamicModInt operator*(DynamicModInt lhs, DynamicModInt rhs);
```

`lhs` と `rhs` の積を返します。

### 計算量

- $O(1)$

## operator/

```cpp
DynamicModInt operator/(DynamicModInt lhs, DynamicModInt rhs);
```

`lhs` を `rhs` で割った商を返します。

### 制約

- $\gcd(\mathrm{rhs}.\mathrm{val}(), m) = 1$

### 計算量

- $O(\log m)$

## operator==

```cpp
bool operator==(DynamicModInt lhs, DynamicModInt rhs);
```

`lhs` と `rhs` が法 $m$ の下で合同であれば `true` を、そうでなければ `false` を返します。

### 計算量

- $O(1)$

## operator!=

```cpp
bool operator!=(DynamicModInt lhs, DynamicModInt rhs);
```

`lhs` と `rhs` が法 $m$ の下で合同でなければ `true` を、そうでなければ `false` を返します。

### 計算量

- $O(1)$

## operator<<

```cpp
std::ostream& operator<<(std::ostream& os, DynamicModInt x);
```

`x.val()` を出力ストリーム `os` へ書き込みます。

### 計算量

- $O(1)$

## operator>>

```cpp
std::istream& operator>>(std::istream& is, DynamicModInt& x);
```

入力ストリーム `is` から空白区切りで文字列を 1 つ読み込み、10 進表記の整数として解釈した値を現在の法 $m$ で割った余りで `x` を更新します。

### 制約

- 読み込む文字列は先頭に `+` または `-` を付けてもよい十進整数表記
- 解釈した値は $-2^{63}$ 以上 $2^{64}$ 未満

### 計算量

- 読み込む文字列の長さを $L$ として $O(L)$

## 補足

- 初期状態では法は `998244353` に設定されています。
- `set_mod` を呼び出す前に生成したオブジェクトの値が新しい法以上である場合の動作は保証されません。

## 使用例

```cpp
#include <iostream>
#include <sstream>

#include "library3372/math/dynamic_modint.hpp"

int main() {
  using Mint = library3372::DynamicModInt<0>;
  Mint::set_mod(1000000007);

  Mint a = 10;
  Mint b = Mint::raw(3);

  std::cout << a + b << "\n";      // 13
  std::cout << a - 20 << "\n";     // 999999997
  std::cout << 2 * a / b << "\n";  // 666666678
  std::cout << a.pow(5) << "\n";   // 100000

  std::istringstream iss("-1");
  Mint c;
  iss >> c;
  std::cout << c << "\n";  // 1000000006
  return 0;
}
```
