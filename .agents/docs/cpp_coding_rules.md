# C++ コーディング規約

- この規約は [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) をベースに作られています。
  - この規約に言及がない事項については、スタイルガイドの内容がそのまま適用されるものとします。
- この規約は、競プロ用ライブラリとして真に保守性の高いコードを実現するために定められています。単に競プロの慣例に反するからという理由でこの規約を破らないでください。

## [ヘッダファイル](https://google.github.io/styleguide/cppguide.html#Header_Files)

- インクルードパスはリポジトリルートからの相対パスを使用してください。
- このライブラリ内のファイルは `#include <filename>` ではなく `#include "filename"` の形式でインクルードしてください。
- ヘッダガードは `#pragma once` ではなく `#ifndef` / `#define` / `#endif` によるガードを使用してください。 
  - ガードシンボルはスタイルガイドに従い `LIBRARY3372_<PATH>_<FILE>_HPP_` の形式にしてください。
  - 例えば `library3372/internal/chrono/timer.hpp` の場合は `LIBRARY3372_LIBRARY3372_INTERNAL_CHRONO_TIMER_HPP_` とします。

### [ヘッダファイル内での関数定義](https://google.github.io/styleguide/cppguide.html#Inline_Functions)

- このライブラリはヘッダベースであり、ヘッダファイル内にすべての関数を実装します。
- ODR 違反を避けるため、クラス定義外の非テンプレート関数には `inline` または `constexpr` を付与してください。

## [スコープ](https://google.github.io/styleguide/cppguide.html#scoping)

### [名前空間](https://google.github.io/styleguide/cppguide.html#namespaces)

- `library3372/` 以下の公開コンポーネントは `library3372` 名前空間に実装します。
- `library3372/internal/` 内のコンポーネントは `library3372::internal` 名前空間に実装します。
- 1 つのヘッダファイル内でのみ利用する内部用コンポーネントは `library3372::internal_<id>` 名前空間に実装します。ここで `<id>` はファイル名に基づく識別子です。
- 必要に応じてその他の名前空間を作成してもかまいません。

## [クラス](https://google.github.io/styleguide/cppguide.html#classes)

### [コピー可能・ムーブ可能な型](https://google.github.io/styleguide/cppguide.html#Copy_Constructors)

- コピー操作・ムーブ操作・デストラクタをいずれもデフォルト定義するクラスでは、これらの宣言を省略してください。
- そうでないクラスでは、いずれも明示的に定義するか `= delete` で削除する必要があります。

### [構造体 vs pair/tuple](https://google.github.io/styleguide/cppguide.html#structs-vs-pairs-and-tuples)

- 関数内で一時的に必要になる場合は `std::pair` / `std::tuple` を使ってもかまいません。
- クラス全体で必要になる場合は構造体を好んで使ってください。

### [アクセス制限](https://google.github.io/styleguide/cppguide.html#access-control)

- 必要な範囲内でデータメンバを `protected` に指定してもかまいません。

### [宣言の順序](https://google.github.io/styleguide/cppguide.html#declaration-order)

- 静的メンバ関数はファクトリの位置にまとめて書いてください。

## [その他の C++ の機能](https://google.github.io/styleguide/cppguide.html#Other_C++_Features)

### [オブジェクトの所有権とスマートポインタ](https://google.github.io/styleguide/cppguide.html#Ownership_and_Smart_Pointers)

- スマートポインタではなく生ポインタを使用します。動的に確保したメモリは手動で解放してください。

### [フレンド](https://google.github.io/styleguide/cppguide.html#friends)

- 異なるファイルに定義されたクラスを `friend` 指定してもかまいません。ただし、今後の設計変更で依存関係が破綻しないか十分に検討してください。

### [例外](https://google.github.io/styleguide/cppguide.html#Exceptions)

- スタイルガイドに従い、C++ の例外機構は使用しません。
- 制約チェックには `<cassert>` の `assert` マクロを使用します。
- 計算量のオーダーを悪化させる `assert` はしないでください。定数倍を顕著に悪化させる `assert` も避けてください。
- `assert` は関数の最初に配置するのが望ましいです。遅くともアサートする制約をアルゴリズム的に使い始めるタイミングまでには済ませるべきです。

### [キャスト](https://google.github.io/styleguide/cppguide.html#Casting)

- 型変換には `static_cast` ではなく C スタイルキャスト `(T)x` を使います。
- 情報が失われる型変換は明示的にキャストして `-Wconversion` による警告が出ないようにしてください。

### [ストリーム](https://google.github.io/styleguide/cppguide.html#streams)

- 以下に該当するクラスについては `operator<<` をオーバーロードする形で出力用 API を定義してください。そうでない場合は出力用 API 自体を提供しません。
  1. そのクラスが定数個のメンバのラッパーであり、
  2. それらのメンバがいずれも `operator<<` で出力可能である。

### [整数型](https://google.github.io/styleguide/cppguide.html#Integer_Types)

- `<cstdint>` の固定幅整数型や `std::size_t` ではなく、以下の組み込み整数型を使用します。
  - 16 / 32 / 64 / 128 bit の符号付き整数型として `short` / `int` / `long long` / `__int128` を使用します。また、同じ bit 長の符号なし整数としてこれらの `unsigned` 型を使用します。
  - 8 bit の整数型として `signed char` / `unsigned char` を使用します。
- `char` は文字の意味で使うものとし、整数型としては使いません。ただし、公開 API 上で対応する必要がある場合は例外です。

### [浮動小数型](https://google.github.io/styleguide/cppguide.html#Floating-Point_Types)

- 仕様上は `long double` / `__float128` を受け付けますが、`long double` / `__float128` の具体的な規格については何も仮定してはいけません。
- `__float128` の計算については `<quadmath.h>` のような非標準のライブラリに頼らず専用のライブラリを内製してください。

### [アーキテクチャ間の移植性](https://google.github.io/styleguide/cppguide.html#Architecture_Portability)

- x64 アーキテクチャをターゲットとする最適化を許可します。
- サポート要件上アーキテクチャ依存の拡張機能が使えるとは限らないので、拡張機能を使わない通常の実装も用意し、プリプロセッサでコードを分岐させてください。

### [非標準の拡張](https://google.github.io/styleguide/cppguide.html#Nonstandard_Extensions)

- コンパイラが GCC であることを仮定できます。特に `__int128` や `__float128` などの拡張型や組み込み関数が利用可能です。
- `-std=gnu++XX` ではなく `-std=c++XX` でコンパイルできるようにしてください。
- `-std=c++XX` 下では `std::integral<__int128>` や `std::floating_point<__float128>` が `false` になるため、独自の型トレイトまたはコンセプトを実装してください。

## [命名規則](https://google.github.io/styleguide/cppguide.html#Naming)

- 以下に従って命名します。
  - 型・型エイリアス・型テンプレート引数・コンセプトの名前は `PascalCase`
  - 変数・定数・関数・名前空間・非型テンプレート引数の名前は `snake_case`
  - マクロの名前はスタイルガイドに準拠
- 標準ライブラリ互換のための型・型エイリアスには `snake_case` を使って問題ありません。
- スタイルガイドに従い、クラスのデータメンバの名前の末尾には `_` を付けてください。
  - 構造体のメンバはこのルールの適用外なので注意してください。

## [コメント](https://google.github.io/styleguide/cppguide.html#Comments)

### [法的通知と著者に関する行](https://google.github.io/styleguide/cppguide.html#legal-notice-and-author-line)

- このプロジェクトで作成する C++ ファイルでは、最初に以下の形式でライセンス表記してください。
  ```cpp
  // Repository: Library3372
  // File: <path>
  // URL: https://github.com/prime3372/Library3372/blob/main/<path>
  // Author: prime3372
  // License: CC0 1.0 Universal
  ```
- このライセンス表記は、ライブラリのユーザーがコンテストの生成 AI 規制ルールに違反するのを防ぐためのものなので、決して省略しないでください。

### [実装に対するコメント](https://google.github.io/styleguide/cppguide.html#implementation-comments)

- アルゴリズムに対する一般的な解説をコメントしないでください。代わりに、大まかな実装方針の名前と、それを解説している記事へのリンクを数行でコメントしてください。
  ```cpp
  // Tarjan's SCC Algorithm
  // https://en.wikipedia.org/wiki/Tarjan%27s_strongly_connected_components_algorithm
  ```
- このライブラリ独自の実装方法に関してコメントすることは問題ありません。

## [フォーマット](https://google.github.io/styleguide/cppguide.html#Formatting)

- `.clang-format` ファイルに従ってフォーマットしてください。フォーマッタの無効化は禁止します。
- コメント行のレイアウトはフォーマットされない設定になっています。コメントを書く際は手動で適切なレイアウトを整えてください。
  - この設定はライセンス表記にフォーマッタがかかってしまうのを防ぐためのものです。
- 単一の文のみを含む制御文の波括弧 `{}` は常に省略可とします。制御文のロジックが単純で、かつコード長も短いときは積極的に省略してください。
