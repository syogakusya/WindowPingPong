# コンパイラの設定
ifeq ($(OS),Windows_NT)
    CXX = g++
    RM = del /Q /S
    EXE = .exe
    SRCDIR = src\\
    SDL_CFLAGS = -IC:/SDL2/include
    SDL_LIBS = -LC:/SDL2/lib -lSDL2main -lSDL2
else
    CXX = g++
    RM = rm -rf
    EXE =
    SRCDIR = src/
    SDL_CFLAGS = $$(sdl2-config --cflags)
    SDL_LIBS = $$(sdl2-config --libs)
endif

# ビルドディレクトリの設定
BUILDDIR = build/

# ディレクトリ作成コマンド
MKDIR = mkdir -p

# コンパイルオプション
CXXFLAGS = -std=c++11 -Wall -Wextra $(SDL_CFLAGS)

# リンクオプション
LDFLAGS = $(SDL_LIBS)

# ソースファイルとオブジェクトファイル
SRCS = $(wildcard $(SRCDIR)*.cpp)
OBJS = $(patsubst $(SRCDIR)%.cpp,$(BUILDDIR)%.o,$(SRCS))

# プロジェクト名（ディレクトリ名から取得）
PROJECT_NAME := $(notdir $(CURDIR))

# 実行ファイル名
TARGET = $(BUILDDIR)$(PROJECT_NAME)$(EXE)

# デフォルトターゲット
all: $(BUILDDIR) $(TARGET)

# ビルドディレクトリの作成
$(BUILDDIR):
	$(MKDIR) $(BUILDDIR)

# 実行ファイルの生成（パスを変更）
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# オブジェクトファイルの生成（出力先を変更）
$(BUILDDIR)%.o: $(SRCDIR)%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# クリーンアップ（ビルドディレクトリを削除）
clean:
	$(RM) $(BUILDDIR)

.PHONY: all clean