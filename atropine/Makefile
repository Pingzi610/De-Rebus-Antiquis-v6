export CROSS ?= arm-none-eabi-
export CC = $(CROSS)gcc
export AS = $(CROSS)as
export OBJCOPY = $(CROSS)objcopy
export AR = $(CROSS)ar
export BIN2C ?= bin2c

export BASE_DIR = $(shell pwd)

export BUILD_MODE ?= RELEASE

export ASFLAGS ?= --fatal-warnings
export CFLAGS ?= -fpie -Wall -nostdlib -std=gnu11 -march=armv7-a -Wno-multichar -Wno-packed-bitfield-compat -ffreestanding -fcall-used-r9 -O3 -mthumb -fno-builtin -DBUILD_MODE='"$(BUILD_MODE)"' -I$(BASE_DIR)
export LDFLAGS ?= $(CFLAGS)


export TARGETS ?= iBoot

.PHONY: all iBoot

all: $(TARGETS)

iBoot:
	@echo Building target iBoot
	@$(MAKE) -C targets/iBoot

clean:
	@$(MAKE) clean -C targets/iBoot