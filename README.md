# Library3372

[prime3372](https://atcoder.jp/users/prime3372) が AI エージェントと一緒に開発している競技プログラミング用ライブラリ

## コンテスト中に利用する方へ

このライブラリは生成 AI を使用して作成されています。コンテストサイトの生成 AI 規制ルールをご確認の上でご利用ください。

## サポート

以下の環境をサポートします。

- C++20 以上
- GCC 13 以上
- `char` / `short` / `int` / `long long` が 8 / 16 / 32 / 64 bit（これらの `unsigned` 型および `signed char` も同じ bit 長）
- 符号付き整数型が 2 の補数で表現される
- `float` / `double` が IEEE 754 規格の binary32 / binary64 フォーマットに従う
- `__int128` / `unsigned __int128` / `__float128` が利用可能

## 使い方

このライブラリをダウンロードして適当なディレクトリに置きます。

`main.cpp` を以下のようにコンパイルすると `main.cpp` 内で `#include "library3372/path/to/file"` が認識されます。

```sh
g++ main.cpp -std=c++20 -I /path/to/Library3372
```

各種 API は `library3372` 名前空間の中に定義されているので、最初に `using namespace library3372;` を書くか、シンボルに `library3372::` を付けてスコープを解決してください。

## ライセンス

CC0 1.0 Universal
