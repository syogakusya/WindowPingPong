# コンパイラの設定
ifeq ($(OS),Windows_NT)
		BUILDDIR = build
		MKDIR = cmd /c If not exist $(BUILDDIR) mkdir
		CXX = g++
		RM = cmd /c rd /Q /S
		EXE = .exe
		SRCDIR = src
		SDL_CFLAGS = -IC:\SDL2\include\SDL2 -IC:\SDL2_ttf\include -Dmain=SDL_main
		SDL_LIBS = -LC:\SDL2\lib -LC:\SDL2_ttf\lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf
else
		BUILDDIR = build
		MKDIR = mkdir -p
		CXX = g++
		RM = rm -rf
		EXE = .exe
		SRCDIR = src
		SDL_CFLAGS = $$(sdl2-config --cflags) $$(pkg-config --cflags SDL2_ttf)
		SDL_LIBS = $$(sdl2-config --libs) $$(pkg-config --libs SDL2_ttf)
endif
# コンパイルオプション
CXXFLAGS = -std=c++17 -Wall -Wextra $(SDL_CFLAGS)

# リンクオプション
LDFLAGS = $(SDL_LIBS)

# ソースファイルとオブジェクトファイル
SRCS = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/*/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRCS))

# プロジェクト名（ディレクトリ名から取得）
PROJECT_NAME := $(notdir $(CURDIR))

# 実行ファイル名
TARGET = $(BUILDDIR)/$(PROJECT_NAME)$(EXE)

# デフォルトターゲット
all: $(BUILDDIR) $(TARGET)

# サブディレクトリの取得とビルドディレクトリの作成
SUBDIRS := $(sort $(dir $(SRCS)))
BUILD_SUBDIRS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SUBDIRS))

# ビルドディレクトリの作成（サブディレクトリを含む）
$(BUILDDIR):
	$(MKDIR) $(BUILD_SUBDIRS)

# オブジェクトファイルの生成（順序依存を修正）
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 実行ファイルの生成
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# クリーンアップ（ビルドディレクトリを削除）
clean:
	$(RM) $(BUILDDIR)

.PHONY: all clean
