CC_LINUX = gcc
CC_WIN = x86_64-w64-mingw32-gcc
CFLAGS = -Wall -Wextra -O2
CFLAGS_WIN = $(CFLAGS) -s
CFLAGS_DEBUG = -Wall -Wextra -g -O0
TARGET = xrtprof
BUILD_DIR_LINUX = build/linux
BUILD_DIR_WIN = build/windows

.PHONY: all clean linux windows debug

all: linux windows

linux: 
	mkdir -p $(BUILD_DIR_LINUX)
	$(CC_LINUX) $(CFLAGS) xrtprof.c -o $(BUILD_DIR_LINUX)/$(TARGET)

windows:
	mkdir -p $(BUILD_DIR_WIN)
	$(CC_WIN) $(CFLAGS_WIN) xrtprof.c -o $(BUILD_DIR_WIN)/$(TARGET).exe

debug:
	$(CC_LINUX) $(CFLAGS_DEBUG) xrtprof.c -o $(TARGET)_g

clean:
	rm -rf build/ $(TARGET)_g