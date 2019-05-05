export AS = as
export CC = gcc
export LD = ld
# -ldl -lpthread -lrt -lgcc_s -lpthread -lc -lm
export TARGET       := chaosfs
export MAIN         := src/chaosfs.c
export CODES        := $(filter-out $(MAIN), $(wildcard src/*.c))
export HEADERS      := $(wildcard *.h)
export INCLUDE      := -I"$(shell pwd)/include"
export CFLAGS       := -O3 -std=gnu11 -Wall
export DEBUG_FLAGS  := -std=gnu11 -Wall

.PHONY: all clean debug default release
default: release
all: default

clean:
	@printf "\e[32mCleaning\e[0m\n"
	@rm -rf build
	@printf "\e[92mClean done\e[0m\n"

release: $(CODES)
	@printf "\e[32mBuilding $(TARGET)\e[0m\n"
	@mkdir -p build
	$(CC) $(CFLAGS) -g $(CODES) $(MAIN) -o $(TARGET) $(INCLUDE) `pkg-config fuse3 --cflags --libs`
	@mv $(TARGET) build/
	@printf "\e[92mBuilt $(TARGET)\e[0m\n"

debug: $(CODES)
	@printf "\e[32mDebug building $(TARGET)\e[0m\n"
	@mkdir -p build
	$(CC) $(DEBUG_FLAGS) -g $(CODES) $(MAIN) -o $(TARGET) $(INCLUDE) `pkg-config fuse3 --cflags --libs`
	@mv chaosfs build/
	@printf "\e[92mDebug built $(TARGET)\e[0m\n"
