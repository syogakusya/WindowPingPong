# クロスプラットフォーム対応のMakefile（WindowsとmacOS）

# ビルド出力ディレクトリ
BUILD_DIR := build

# シェルの設定
ifeq ($(OS),Windows_NT)
    PLATFORM := windows
    SHELL := cmd.exe
    PATH_SEPARATOR := /
else
    PLATFORM := mac
    SHELL := /bin/sh
    PATH_SEPARATOR := /
endif

# 共通の設定
CC := g++
CXX := g++
CFLAGS := -std=c++17 -Wall -Wextra
SRCS := $(wildcard src/**/*.cpp src/*.cpp)
OBJS := $(patsubst src/%.cpp,$(BUILD_DIR)/src/%.o,$(SRCS))

# プラットフォーム固有の設定
ifeq ($(PLATFORM), windows)
    SDL2_DIR := libs/SDL2/windows
    SDL2_TTF_DIR := libs/SDL2_ttf/windows
    SDL2_MIXER_DIR := libs/SDL2_mixer/windows
    CFLAGS += -I$(SDL2_DIR)/include/SDL2 -I$(SDL2_TTF_DIR)/include/SDL2 -I$(SDL2_MIXER_DIR)/include/SDL2
    LDFLAGS := -L$(SDL2_DIR)/lib -L$(SDL2_TTF_DIR)/lib -L$(SDL2_MIXER_DIR)/lib \
               -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_mixer -mwindows
    TARGET := $(BUILD_DIR)/WindowPingPong.exe
    DLLS := $(SDL2_DIR)/lib/SDL2.dll $(SDL2_TTF_DIR)/lib/SDL2_ttf.dll $(SDL2_MIXER_DIR)/lib/SDL2_mixer.dll
    ifeq ($(SHELL),cmd.exe)
        MKDIR := mkdir
        COPY := copy
        CLEAN_CMD := rmdir /S /Q $(BUILD_DIR)
    else
        MKDIR := mkdir -p
        COPY := cp
        CLEAN_CMD := rm -rf $(BUILD_DIR)
    endif
endif

# ビルドディレクトリの設定
BUILD_DIRS := \
    $(BUILD_DIR)/src/GameObjects \
    $(BUILD_DIR)/src/Scenes \
    $(BUILD_DIR)/src/UI \
    $(BUILD_DIR)/src/Utils

# すべてのターゲットをビルド
all: $(BUILD_DIR) $(TARGET)

# ビルドディレクトリの作成
$(BUILD_DIR):
ifeq ($(SHELL),cmd.exe)
	@for %%d in ($(BUILD_DIRS)) do ($(MKDIR) "%%d")
else
	@$(foreach dir,$(BUILD_DIRS),$(MKDIR) "$(dir)";)
endif
	@echo "ディレクトリを作成しました"

# コピーコマンドの定義
ifeq ($(PLATFORM), windows)
ifeq ($(SHELL),cmd.exe)
COPY_DLLS := \
    $(COPY) "$(SDL2_DIR)\bin\SDL2.dll" "$(BUILD_DIR)\" & \
    $(COPY) "$(SDL2_TTF_DIR)\bin\SDL2_ttf.dll" "$(BUILD_DIR)\" & \
    $(COPY) "$(SDL2_MIXER_DIR)\bin\SDL2_mixer.dll" "$(BUILD_DIR)\"
else
COPY_DLLS := $(COPY) $(DLLS) $(BUILD_DIR)/
endif
endif

# ターゲットのリンク
ifeq ($(PLATFORM), mac)
$(TARGET): $(OBJS)
	@mkdir -p $(APP_DIRS)
	$(CXX) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	# Info.plistの作成（省略）
else ifeq ($(PLATFORM), windows)
$(TARGET): $(OBJS)
	$(CXX) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	$(COPY_DLLS)
endif

# オブジェクトファイルのコンパイルルール
$(BUILD_DIR)/src/%.o: src/%.cpp
ifeq ($(SHELL),cmd.exe)
	@if not exist "$(dir $@)" $(MKDIR) "$(dir $@)"
else
	@$(MKDIR) -p "$(dir $@)"
endif
	$(CXX) $(CFLAGS) -c $< -o $@

# クリーンアップ
clean:
	$(CLEAN_CMD)

# 実行
run: $(TARGET)
ifeq ($(PLATFORM), windows)
	$(TARGET)
else ifeq ($(PLATFORM), mac)
	open $(APP_BUNDLE)
endif

# デバッグ実行
debug: $(TARGET)
ifeq ($(PLATFORM), windows)
	$(TARGET) --debug
else ifeq ($(PLATFORM), mac)
	$(TARGET) --debug
endif

# 擬似ターゲットの宣言
.PHONY: all clean run debug
