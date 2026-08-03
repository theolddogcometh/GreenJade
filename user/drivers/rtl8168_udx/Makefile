# SPDX-License-Identifier: MIT OR Apache-2.0
# Local host build for rtl8168_udx soft skeleton.
# Prefer: make -C user/udx lib  (or repo-root: make udx) then this Makefile.

UDX_DIR  := $(abspath ../../udx)
BUILD    ?= $(abspath build)
CC       ?= gcc

UDX_INC  ?= $(UDX_DIR)/include
UDX_LIB  ?= $(UDX_DIR)/build/libudx.a

UDX_CFLAGS := -std=c11 -Wall -Wextra -Werror -I$(UDX_INC) -O2 -g -DUDX_HOST_LIBC=1

BIN := $(BUILD)/rtl8168_udx

.PHONY: all clean libudx

all: $(BIN)

libudx:
	$(MAKE) -C $(UDX_DIR) lib

$(UDX_LIB):
	$(MAKE) -C $(UDX_DIR) lib

$(BIN): rtl8168_udx.c $(UDX_LIB)
	@mkdir -p $(dir $@)
	$(CC) $(UDX_CFLAGS) -o $@ rtl8168_udx.c $(UDX_LIB)
	@echo "built $@  (run: ./$@)"

clean:
	rm -rf $(BUILD)
