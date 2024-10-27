# WindowsPingPong

SDL2 を使用したマルチウィンドウ Pong ゲーム

## 概要

このゲームは、SDL2 を使用して作られた特殊な Pong ゲームです。通常の 1 画面でのゲームと異なり、パドル、ボール、スコア表示がそれぞれ独立したウィンドウとして表示されます。

## 必要条件

- SDL2
- SDL2_ttf
- C++コンパイラ（C++17 以上）

## ビルド方法

```bash
% make
```

## 実行方法

### Windows

```bash
./WindowsPingPong.exe
```

### macOS

```bash
./WindowsPingPong.app/Contents/MacOS/WindowsPingPong
```

or

```bash
open ./WindowsPingPong.app
```

デバッグ出力を確認したい場合は以下のコマンドを使用してください：

```bash
./WindowPingPong.app/Contents/MacOS/WindowPingPong 2>&1 | tee output.log
```

## 操作方法

- W/S または ↑/↓: パドルの移動
- スペース: 一時停止
- ESC: ゲーム終了

## ライセンス

このプロジェクトは SDL2 の zlib ライセンスに従います。詳細は[LICENSE.md](LICENSE.md)を参照してください。
