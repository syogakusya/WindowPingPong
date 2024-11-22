# ビルド出力ディレクトリ
BUILD_DIR := build

# シェルの設定
ifeq ($(OS),Windows_NT)
    PLATFORM := windows
    SHELL := cmd.exe
    PATH_SEPARATOR := /
    COPY := xcopy /E /I /Y
else
    PLATFORM := mac
    SHELL := /bin/sh
    PATH_SEPARATOR := /
    COPY := cp -r
endif

# アセットのディレクトリ
ASSETS := assets$(PATH_SEPARATOR)fonts assets$(PATH_SEPARATOR)sounds assets$(PATH_SEPARATOR)images

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
        MKDIR := mkdir /p
        CLEAN_CMD := rmdir /S /Q $(BUILD_DIR)
    else
        MKDIR := mkdir -p
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
all: $(BUILD_DIR) copy_assets $(TARGET)

# ビルドディレクトリの作成
$(BUILD_DIR):
ifeq ($(SHELL),cmd.exe)
	@for %%d in ($(BUILD_DIRS)) do ($(MKDIR) "%%d")
else
	@$(foreach dir,$(BUILD_DIRS),$(MKDIR) "$(dir)";)
endif
	@echo "ディレクトリを作成しました"

# アセットをビルドディレクトリにコピー
copy_assets:
ifeq ($(PLATFORM), windows)
	@echo "アセットをコピーしています..."
	$(foreach dir,$(ASSETS), \
		$(COPY) "$(dir)" "$(BUILD_DIR)$(PATH_SEPARATOR)$(dir)" &)
	@echo "アセットをコピーしました"
else
	@$(foreach dir,$(ASSETS), \
		$(MKDIR) "$(BUILD_DIR)/$(dir)"; \
		$(COPY) "$(dir)/*" "$(BUILD_DIR)/$(dir)/";)
	@echo "アセットをコピーしました"
endif

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
	@echo "Info.plistを作成しています..."
	@echo '<?xml version="1.0" encoding="UTF-8"?>' > $(APP_BUNDLE)/Contents/Info.plist
	@echo '<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">' >> $(APP_BUNDLE)/Contents/Info.plist
	@echo '<plist version="1.0">' >> $(APP_BUNDLE)/Contents/Info.plist
	@echo '<dict>' >> $(APP_BUNDLE)/Contents/Info.plist
	@echo '  <key>CFBundleExecutable</key>' >> $(APP_BUNDLE)/Contents/Info.plist
	@echo '  <string>$(APP_NAME)</string>' >> $(APP_BUNDLE)/Contents/Info.plist
	@echo '  <key>CFBundleIdentifier</key>' >> $(APP_BUNDLE)/Contents/Info.plist
	@echo '  <string>com.example.$(APP_NAME)</string>' >> $(APP_BUNDLE)/Contents/Info.plist
	@echo '  <key>CFBundleName</key>' >> $(APP_BUNDLE)/Contents/Info.plist
	@echo '  <string>$(APP_NAME)</string>' >> $(APP_BUNDLE)/Contents/Info.plist
	@echo '  <key>CFBundleVersion</key>' >> $(APP_BUNDLE)/Contents/Info.plist
	@echo '  <string>1.0</string>' >> $(APP_BUNDLE)/Contents/Info.plist
	@echo '</dict>' >> $(APP_BUNDLE)/Contents/Info.plist
	@echo '</plist>' >> $(APP_BUNDLE)/Contents/Info.plist
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

# クリーンビルド用のディクトリ
CLEAN_BUILD_DIR := clean_build

# クリーンビルドの作成
clean_build: clean
	$(MKDIR) $(CLEAN_BUILD_DIR)
ifeq ($(SHELL),cmd.exe)
	@for %%d in ($(ASSETS)) do (
		$(MKDIR) "$(CLEAN_BUILD_DIR)\%%d"
		$(COPY) "%%d\*" "$(CLEAN_BUILD_DIR)\%%d\"
	)
	$(COPY) "$(TARGET)" "$(CLEAN_BUILD_DIR)\"
else
	@$(foreach dir,$(ASSETS),$(MKDIR) "$(CLEAN_BUILD_DIR)/$(dir)"; \
		$(COPY) "$(dir)/*" "$(CLEAN_BUILD_DIR)/$(dir)/";)
	$(COPY) $(TARGET) $(CLEAN_BUILD_DIR)/
endif
	@echo "クリーンビルドを作成しました: $(CLEAN_BUILD_DIR)"
