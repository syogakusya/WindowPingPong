# Windows Ping Pong

![Game Screenshot](path/to/screenshot.png)

## 概要

Windows Ping Pongは、クラシックなピンポンゲームに現代的なツイストを加えた、ユニークなWindowsデスクトップゲームです。
従来のピンポンゲームの基本的な要素を保ちながら、Windowsのウィンドウシステムを活用した革新的なゲームプレイを提供します。

### 特徴

- **ウィンドウベースのゲームプレイ**: 各ゲーム要素（ボール、パドル）が独立したウィンドウとして動作
- **妨害ウィンドウシステム**: ポップアップウィンドウやシステムメッセージが障害物として出現
- **マルチボールシステム**: スコアに応じて増加するボール
- **動的な難易度調整**: プレイ時間とともに上昇する難易度

## 技術スタック

- **言語**: C++17
- **グラフィックスライブラリ**: SDL2
- **ビルドシステム**: Make
- **開発環境**: Visual Studio Code

## 実装の特徴

### アーキテクチャ設計
- Scene管理による画面遷移の実装
- GameObjectベースの継承設計
- コンポーネントパターンの活用

### 最適化
- スマートポインタによるメモリ管理
- 効率的な衝突判定システム
- フレームレート独立な物理演算

### クリーンコード
- SOLID原則に基づいた設計
- 適切な責務分離
- 詳細なコメント記述

## 開発における工夫

1. **パフォーマンス最適化**
   - ウィンドウ操作のオーバーヘッド最小化
   - 効率的なメモリ管理
   - フレームレート安定化

2. **ユーザビリティ向上**
   - 直感的な操作方法
   - 視覚的フィードバック
   - スムーズな動���感

3. **拡張性**
   - 新機能追加が容易な設計
   - モジュール化されたコード構造
   - 設定の外部化

## インストール方法

```bash
git clone https://github.com/yourusername/windows-ping-pong.git
cd windows-ping-pong
make
```

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

- **W/S** または **↑/↓**: パドル移動
- **Space**: 一時停止
- **C**: マウス追従モード切り替え
- **ESC**: ゲーム終了

## 今後の展望

- オンラインマルチプレイヤー対応
- カスタマイズ可能な妨害ウィンドウシステム
- アチーブメントシステムの実装
- より高度なAIの実装

## 開発者

- 名前: [あなたの名前]
- GitHub: [@yourusername](https://github.com/yourusername)
- Email: your.email@example.com

## ライセンス

このプロジェクトは[MITライセンス](LICENSE.md)の下で公開されています。

## ビルドシステム

### Makefileの特徴

- **クロスプラットフォーム対応**
  - Windows/macOS/Linux環境での自動的な環境検出
  - プラットフォーム固有の設定の自動切り替え
  - SDL2ライブラリのパスを環境に応じて適切に設定

- **柔軟なビルド設定**
  - C++17標準の採用
  - デバッグ情報の埋め込み
  - 警告レベルの最適化（-Wall -Wextra）

- **macOS向けの特別な対応**
  - .appバンドルの自動生成
  - 必要なフレームワーク（Cocoa, Metal, MetalKit）の自動リンク
  - Info.plistの自動生成

### 主要なMakeコマンド

```bash
# 通常のビルド
make

# ビルドとクリーン
make clean    # ビルドファイルの削除
make all      # 完全なリビルド

# 実行
make run      # ビルドして実行

# macOS専用コマンド
make bundle   # .appバンドルの作成
make debug    # デバッグ出力付きで実行
make lldebug  # LLDBでデバッグ
```

### ディレクトリ構造

```
project_root/
├── src/
│   ├── Game.cpp
│   ├── Game.h
│   ├── Main.cpp
│   ├── GameObjects/
│   │   ├── Ball.cpp
│   │   ├── Ball.h
│   │   ├── GameObject.cpp
│   │   ├── GameObject.h
│   │   ├── Paddle.cpp
│   │   └── Paddle.h
│   ├── Scenes/
│   │   ├── GamePlayScene.cpp
│   │   ├── GamePlayScene.h
│   │   ├── Scene.h
│   │   ├── SceneManager.cpp
│   │   ├── SceneManager.h
│   │   ├── StartScene.cpp
│   │   └── StartScene.h
│   └── Utils/
│       ├── TextRenderer.cpp
│       └── TextRenderer.h
├── fonts/
│   └── PixelifySans-VariableFont_wght.ttf
├── Makefile
└── README.md
```

### 依存関係

- **必須ライブラリ**
  - SDL2
  - SDL2_ttf

#### Windows環境での設定
```bash
# SDL2とSDL2_ttfをC:\SDL2とC:\SDL2_ttfにインストール
```

#### macOS環境での設定
```bash
# Homebrewを使用してインストール
brew install sdl2 sdl2_ttf
```

#### Linux環境での設定
```bash
# Ubuntuの場合
sudo apt-get install libsdl2-dev libsdl2-ttf-dev

# Fedoraの場合
sudo dnf install SDL2-devel SDL2_ttf-devel
```

