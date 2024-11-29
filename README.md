# Windows Ping Pong

## 概要

Windows Ping Pong は、クラシックなピンポンゲームに現代的なツイストを加えた、ユニークな Windows デスクトップゲームです。
従来のピンポンゲームの基本的な要素を保ちながら、Windows のウィンドウシステムを活用した革新的なゲームプレイを提供します。

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

- Scene 管理による画面遷移の実装
- GameObject ベースの継承設計
- コンポーネントパターンの活用

### 最適化

- スマートポインタによるメモリ管理
- 効率的な衝突判定システム
- フレームレート独立な物理演算

### クリーンコード

- SOLID 原則に基づいた設計
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
   - スムーズな動 ��� 感

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
- **C**: マウス追従モード切り替え
- **ESC**: ゲーム終了

## 今後の展望

- オンラインマルチプレイヤー対応
- カスタマイズ可能な妨害ウィンドウシステム
- アチーブメントシステムの実装
- より高度な AI の実装

## 開発者

- 名前: [土田悠太]
- GitHub: [@syogakusya](https://github.com/syogakusya)
- Email: chitsuoman404@gmail.com

## ライセンス

このプロジェクトは[LICENSE](LICENSE.md)の下で公開されています。

## ビルドシステム

### Makefile の特徴

- **クロスプラットフォーム対応**

  - Windows/macOS/Linux 環境での自動的な環境検出
  - プラットフォーム固有の設定の自動切り替え
  - SDL2 ライブラリのパスを環境に応じて適切に設定

- **柔軟なビルド設定**

  - C++17 標準の採用
  - デバッグ情報の埋め込み
  - 警告レベルの最適化（-Wall -Wextra）

- **macOS 向けの特別な対応**
  - .app バンドルの自動生成
  - 必要なフレームワーク（Cocoa, Metal, MetalKit）の自動リンク
  - Info.plist の自動生成

### 主要な Make コマンド

```bash
# 通常のビルド
make

# ビルドとクリーン
make clean    # ビルドファイルの削除
make all      # 完全なリビルド

# 実行
make run      # ビルドして実行
make debug    # デバッグ出力付きで実行
make dist     # 配布用アーカイブの作成

# macOS専用コマンド
make bundle   # .appバンドルの作成
make lldebug  # LLDBでデバッグ
```

### 依存関係

- **必須ライブラリ**
  - SDL2
  - SDL2_ttf
  - SDL2_image
  - SDL2_mixer

#### Windows 環境での設定

必要なライブラリは既にプロジェクトに含まれています。
MSYS2 で make をインストールして下さい。

#### macOS 環境での設定

```bash
# Homebrewを使用してインストール
brew install sdl2 sdl2_ttf
```
