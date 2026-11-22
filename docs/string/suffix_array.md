# Suffix Array

文字列や配列の接尾辞配列を計算する関数です。長さ $n$ の列に対し、$n$ 個のすべての接尾辞を辞書順に並べ替えた開始位置の配列を返します。

## suffix_array

```cpp
std::vector<int> suffix_array(std::string_view s);                    // (1)
std::vector<int> suffix_array(const std::vector<int>& s, int upper);  // (2)

template <typename T>
std::vector<int> suffix_array(const std::vector<T>& s);  // (3)

template <typename T, typename Compare>
std::vector<int> suffix_array(const std::vector<T>& s, Compare compare);  // (4)
```

長さ $n$ の列 `s` に対する接尾辞配列を長さ $n$ の `std::vector<int>` として返します。戻り値 `sa` は $0, 1, \dots, n - 1$ の順列であり、`s[sa[i]..]` が辞書順で $i$ 番目の接尾辞になります。(4) では比較関数として `compare` を指定します。

### 制約

- $n < 2^{30}$
- (2): $0 \leq s_i \leq \mathrm{upper}$
- (3): `operator<` は要素間の狭義弱順序を定める二項述語
- (4): `compare` は要素間の狭義弱順序を定める二項述語

### 計算量

- (1): $O(n)$
- (2): $O(n + \mathrm{upper})$
- (3): `T` が整数型の場合は $O(n)$、そうでない場合は $O(n \log n)$
- (4): $O(n \log n)$

## 使用例

```cpp
#include <iostream>
#include <string>
#include <vector>

#include "library3372/string/suffix_array.hpp"

int main() {
  std::string s = "mississippi";
  std::vector<int> sa = library3372::suffix_array(s);

  for (int i : sa) std::cout << i << " ";
  std::cout << "\n";
  // 10 7 4 1 0 9 8 6 3 5 2
  return 0;
}
```
