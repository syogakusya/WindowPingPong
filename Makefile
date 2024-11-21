# プロジェクト名（ディレクトリ名から取得）
PROJECT_NAME := $(notdir $(CURDIR))

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
		SDL_CFLAGS = $$(sdl2-config --cflags) $$(pkg-config --cflags SDL2_ttf) $$(pkg-config --cflags SDL2_mixer)
		SDL_LIBS = $$(sdl2-config --libs) $$(pkg-config --libs SDL2_ttf) $$(pkg-config --libs SDL2_mixer)
endif

# コンパイルオプション
CXXFLAGS = -std=c++17 -Wall -Wextra -g $(SDL_CFLAGS)

# macOS向けの追加設定
ifeq ($(shell uname),Darwin)
	LDFLAGS += -framework Cocoa -framework Metal -framework MetalKit
	APP_NAME = $(PROJECT_NAME).app
	APP_CONTENTS = $(APP_NAME)/Contents
	APP_MACOS = $(APP_CONTENTS)/MacOS
	APP_RESOURCES = $(APP_CONTENTS)/Resources
endif

# リンクオプション
LDFLAGS = $(SDL_LIBS)

# ソースファイルとオブジェクトファイル
SRCS = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/*/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRCS))

# 実行ファイル名
TARGET = $(BUILDDIR)/$(PROJECT_NAME)$(EXE)

# サブディレクトリの取得とビルドディレクトリの作成
SUBDIRS := $(sort $(dir $(SRCS)))
BUILD_SUBDIRS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SUBDIRS))

# フォントファイルのパスを定義（プロジェクトのルートディレクトリからの相対パス）
FONT_PATH = fonts

.PHONY: all clean bundle

# デフォルトターゲット
all: $(BUILDDIR) $(TARGET)
ifeq ($(shell uname),Darwin)
	$(MAKE) bundle
endif

# ビルドディレクトリの作成
$(BUILDDIR):
	@$(MKDIR) $(BUILD_SUBDIRS)

# オブジェクトファイルの生成
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# 実行ファイルの生成
$(TARGET): $(OBJS)
	@echo "Linking $@..."
	@$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# macOS用の.appバンドル作成
bundle:
ifeq ($(shell uname),Darwin)
	@echo "Creating application bundle..."
	@mkdir -p $(APP_MACOS)
	@mkdir -p $(APP_RESOURCES)
	@mkdir -p $(APP_RESOURCES)/fonts
	@cp $(TARGET) $(APP_MACOS)/$(PROJECT_NAME)
	@cp $(FONT_PATH)/*.ttf $(APP_RESOURCES)/fonts/
	@echo "Copying fonts to Resources directory..."
	@echo '<?xml version="1.0" encoding="UTF-8"?>\
	<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">\
	<plist version="1.0">\
	<dict>\
		<key>CFBundleExecutable</key>\
			<string>$(PROJECT_NAME)</string>\
		<key>CFBundleIdentifier</key>\
			<string>com.example.$(PROJECT_NAME)</string>\
		<key>CFBundleName</key>\
			<string>$(PROJECT_NAME)</string>\
		<key>CFBundlePackageType</key>\
			<string>APPL</string>\
		<key>CFBundleShortVersionString</key>\
			<string>1.0</string>\
		<key>LSMinimumSystemVersion</key>\
			<string>10.10</string>\
	</dict>\
	</plist>' > $(APP_CONTENTS)/Info.plist
	@echo "Application bundle created: $(APP_NAME)"
endif

# クリーンアップ
clean:
	@echo "Cleaning build directory..."
	@$(RM) $(BUILDDIR)
ifeq ($(shell uname),Darwin)
	@echo "Cleaning application bundle..."
	@$(RM) $(APP_NAME)
endif

run:
	@echo "Running $(TARGET)..."
ifeq ($(shell uname),Darwin)
	@open ./WindowPingPong.app
else
	@./build/WindowPingPong.exe
endif

ifeq ($(shell uname),Darwin)
debug:
	@./WindowPingPong.app/Contents/MacOS/WindowPingPong 2>&1

lldebug:
	lldb ./WindowPingPong.app/Contents/MacOS/WindowPingPong
endif

