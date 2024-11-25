# ビルド出力ディレクトリ
BUILD_DIR := build

# 改行の定義
define newline


endef

# シェルの設定
ifeq ($(OS),Windows_NT)
    PLATFORM := windows
    SHELL := cmd.exe
    COPY := xcopy /E /I /Y
    MKDIR := mkdir
    MKDIROPTIONS :=
else
    PLATFORM := mac
    SHELL := /bin/sh
    COPY := cp -r
    MKDIR := mkdir
    MKDIROPTIONS := -p
endif

# アセットのディレクトリ
ASSETS := assets/fonts assets/sounds assets/images

# 共通の設定
CC := g++
CXX := g++
CFLAGS := -std=c++17 -Wall -Wextra

# ソースファイルのリスト
SRCS := $(wildcard src/*.cpp src/*/*.cpp src/*/*/*.cpp)
# オブジェクトファイルのパスを定義
OBJS := $(patsubst src/%.cpp,$(BUILD_DIR)/src/%.o,$(SRCS))

# プラットフォーム固有の設定
ifeq ($(PLATFORM), windows)
    SDL2_DIR := libs/SDL2/windows
    SDL2_TTF_DIR := libs/SDL2_ttf/windows
    SDL2_MIXER_DIR := libs/SDL2_mixer/windows
    SDL2_IMAGE_DIR := libs/SDL2_image/windows
    CFLAGS += -I$(SDL2_DIR)/include/SDL2 -I$(SDL2_TTF_DIR)/include/SDL2 -I$(SDL2_MIXER_DIR)/include/SDL2 -I$(SDL2_IMAGE_DIR)/include/SDL2
    LDFLAGS := -L$(SDL2_DIR)/lib -L$(SDL2_TTF_DIR)/lib -L$(SDL2_MIXER_DIR)/lib -L$(SDL2_IMAGE_DIR)/lib \
               -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_mixer -lSDL2_image
    LDFLAGS_DEBUG += $(LDFLAGS)
    LDFLAGS_RELEASE += $(LDFLAGS) -mwindows
    TARGET := $(BUILD_DIR)/WindowPingPong.exe
    DLLS := "$(SDL2_DIR)/lib/SDL2.dll" "$(SDL2_TTF_DIR)/lib/SDL2_ttf.dll" "$(SDL2_MIXER_DIR)/lib/SDL2_mixer.dll"
    CLEAN_CMD := rmdir /S /Q "$(BUILD_DIR)"
else ifeq ($(PLATFORM), mac)
    CFLAGS += $(shell sdl2-config --cflags) $(shell pkg-config --cflags SDL2_ttf SDL2_mixer)
    LDFLAGS := $(shell sdl2-config --libs) $(shell pkg-config --libs SDL2_ttf SDL2_mixer)
    LDFLAGS_DEBUG += $(LDFLAGS)
    LDFLAGS_RELEASE += $(LDFLAGS)
    APP_NAME := WindowPingPong
    APP_BUNDLE := $(BUILD_DIR)/$(APP_NAME).app
    TARGET := $(APP_BUNDLE)/Contents/MacOS/$(APP_NAME)
    APP_DIRS := "$(APP_BUNDLE)/Contents/MacOS" "$(APP_BUNDLE)/Contents/Resources"
    CLEAN_CMD := rm -rf "$(BUILD_DIR)"
endif

# デバッグとリリース用のフラグ
CFLAGS_DEBUG := $(CFLAGS) -DDEBUG
CFLAGS_RELEASE := $(CFLAGS)

# ビルドディレクトリの設定
BUILD_DIRS := $(sort $(dir $(OBJS)))

# すべてのターゲットをビルド
all: CFLAGS := $(CFLAGS_RELEASE)
all: LDFLAGS := $(LDFLAGS_RELEASE)
all: $(BUILD_DIR) copy_assets $(TARGET)

# ビルドディレクトリの作成
$(BUILD_DIR):
	@echo Creating directories...
	@if not exist "$(BUILD_DIR)" $(MKDIR) $(MKDIROPTIONS) "$(BUILD_DIR)"
ifeq ($(PLATFORM), windows)
	$(foreach dir,$(BUILD_DIRS), $(MKDIR) $(MKDIROPTIONS) "$(dir)"$(newline))
else
	$(MKDIR) $(MKDIROPTIONS) $(BUILD_DIRS)
endif
	@echo "ディレクトリを作成しました"

# アセットをビルドディレクトリにコピー
copy_assets:
	@echo "アセットをコピーしています..."
ifeq ($(PLATFORM), windows)
	$(foreach dir,$(ASSETS), \
		@if not exist "$(BUILD_DIR)/$(dir)" $(MKDIR) $(MKDIROPTIONS) "$(BUILD_DIR)/$(dir)"$(newline)\
		$(COPY) "$(dir)" "$(BUILD_DIR)/$(dir)"$(newline))
else
	$(foreach dir,$(ASSETS), \
		$(MKDIR) $(MKDIROPTIONS) "$(BUILD_DIR)/$(dir)"; \
		$(COPY) "$(dir)/" "$(BUILD_DIR)/$(dir)/";)
endif
	@echo "アセットをコピーしました"

# DLLのコピー（Windows用）
ifeq ($(PLATFORM), windows)
COPY_DLLS := \
    $(COPY) "$(SDL2_DIR)\bin\SDL2.dll" "$(BUILD_DIR)\"$(newline)\
    $(COPY) "$(SDL2_TTF_DIR)\bin\SDL2_ttf.dll" "$(BUILD_DIR)\"$(newline)\
    $(COPY) "$(SDL2_MIXER_DIR)\bin\SDL2_mixer.dll" "$(BUILD_DIR)\"$(newline)\
    $(COPY) "$(SDL2_IMAGE_DIR)\bin\SDL2_image.dll" "$(BUILD_DIR)\"
endif

# ターゲットのリンク
$(TARGET): $(OBJS)
ifeq ($(PLATFORM), windows)
	$(CXX) $(CFLAGS) -o "$(TARGET)" $(OBJS) $(LDFLAGS)
	$(COPY_DLLS)
else ifeq ($(PLATFORM), mac)
	@mkdir -p $(APP_DIRS)
	$(CXX) $(CFLAGS) -o "$(TARGET)" $(OBJS) $(LDFLAGS)
	@echo "Info.plistを作成しています..."
	@echo '<?xml version="1.0" encoding="UTF-8"?>' > "$(APP_BUNDLE)/Contents/Info.plist"
	@echo '<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">' >> "$(APP_BUNDLE)/Contents/Info.plist"
	@echo '<plist version="1.0">' >> "$(APP_BUNDLE)/Contents/Info.plist"
	@echo '<dict>' >> "$(APP_BUNDLE)/Contents/Info.plist"
	@echo '  <key>CFBundleExecutable</key>' >> "$(APP_BUNDLE)/Contents/Info.plist"
	@echo '  <string>$(APP_NAME)</string>' >> "$(APP_BUNDLE)/Contents/Info.plist"
	@echo '  <key>CFBundleIdentifier</key>' >> "$(APP_BUNDLE)/Contents/Info.plist"
	@echo '  <string>com.example.$(APP_NAME)</string>' >> "$(APP_BUNDLE)/Contents/Info.plist"
	@echo '  <key>CFBundleName</key>' >> "$(APP_BUNDLE)/Contents/Info.plist"
	@echo '  <string>$(APP_NAME)</string>' >> "$(APP_BUNDLE)/Contents/Info.plist"
	@echo '  <key>CFBundleVersion</key>' >> "$(APP_BUNDLE)/Contents/Info.plist"
	@echo '  <string>1.0</string>' >> "$(APP_BUNDLE)/Contents/Info.plist"
	@echo '</dict>' >> "$(APP_BUNDLE)/Contents/Info.plist"
	@echo '</plist>' >> "$(APP_BUNDLE)/Contents/Info.plist"
endif

# オブジェクトファイルのコンパイルルール
$(BUILD_DIR)/src/%.o: src/%.cpp
	$(CXX) $(CFLAGS) -c "$<" -o "$@"

# クリーンアップ
clean:
	$(CLEAN_CMD)

# 実行
run: CFLAGS := $(CFLAGS_RELEASE)
run: LDFLAGS := $(LDFLAGS_RELEASE)
run: $(TARGET)
ifeq ($(PLATFORM), windows)
	@"$(TARGET)"
else ifeq ($(PLATFORM), mac)
	@open "$(APP_BUNDLE)"
endif

# デバッグ実行
debug: CFLAGS := $(CFLAGS_DEBUG)
debug: LDFLAGS := $(LDFLAGS_DEBUG)
debug: $(BUILD_DIR) copy_assets $(TARGET)
ifeq ($(PLATFORM), windows)
	@"$(TARGET)"
else ifeq ($(PLATFORM), mac)
	@open "$(APP_BUNDLE)"
endif

# 配布用のアーカイブを作成するターゲット
dist: all
	@echo "配布用アーカイブを作成しています..."
ifeq ($(PLATFORM), windows)
	@if not exist dist mkdir dist
	@xcopy /E /I /Y "$(BUILD_DIR)" dist
	@if exist README.md copy README.md dist
	@if exist LICENSE.md copy LICENSE.md dist
	@del /Q "dist\*.dll"
	@rmdir /S /Q dist\src
	@powershell -Command "Compress-Archive -Path 'dist\*' -DestinationPath 'WindowPingPong.zip'"
	@rmdir /S /Q dist
else
	@mkdir -p dist
	@cp -r "$(BUILD_DIR)" dist/
	@cp README.md dist
	@cp LICENSE dist
	@cd dist && zip -r ../WindowPingPong.zip *
	@rm -rf dist
endif
	@echo "配布用アーカイブ WindowPingPong.zip を作成しました"

# 擬似ターゲットの宣言
.PHONY: all clean run debug
