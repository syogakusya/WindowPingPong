# コンパイラの設定
ifeq ($(OS),Windows_NT)
		BUILDDIR = build
		MKDIR = cmd /c If not exist $(BUILDDIR) mkdir
		CXX = g++
		RM = cmd /c rd /Q /S
		EXE = .exe
		SRCDIR = src
		SDL_CFLAGS = -IC:\SDL2\include\SDL2 -Dmain=SDL_main
		SDL_LIBS = -LC:\SDL2\lib -lmingw32 -lSDL2main -lSDL2
else
		BUILDDIR = build/
		MKDIR = mkdir -p
		CXX = g++
		RM = rm -rf
		EXE = .exe
		SRCDIR = src/
		SDL_CFLAGS = $$(sdl2-config --cflags)
		SDL_LIBS = $$(sdl2-config --libs)
endif
# コンパイルオプション
CXXFLAGS = -std=c++11 -Wall -Wextra $(SDL_CFLAGS)

# リンクオプション
LDFLAGS = $(SDL_LIBS)

# ソースファイルとオブジェクトファイル
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRCS))

a:
	cmd.exe /c  echo $(OBJS)

# プロジェクト名（ディレクトリ名から取得）
PROJECT_NAME := $(notdir $(CURDIR))

# 実行ファイル名
TARGET = $(BUILDDIR)/$(PROJECT_NAME)$(EXE)

# デフォルトターゲット
all: $(BUILDDIR) $(TARGET)

# ビルドディレクトリの作成
$(BUILDDIR):
	$(MKDIR) $(BUILDDIR)

# オブジェクトファイルの生成
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 実行ファイルの生成
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# クリーンアップ（ビルドディレクトリを削除）
clean:
	$(RM) $(BUILDDIR)

.PHONY: all clean
