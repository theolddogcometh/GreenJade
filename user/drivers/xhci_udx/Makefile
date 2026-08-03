# SPDX-License-Identifier: MIT OR Apache-2.0
# Host soft build for xhci_udx against user/udx libudx.
# Prefer: make -C user/udx lib  then  make -C user/drivers/xhci_udx

ROOT      := $(abspath ../../..)
UDX_DIR   := $(abspath ../../udx)
BUILD     ?= $(abspath build)
CC        ?= gcc

UDX_CFLAGS := -std=c11 -Wall -Wextra -Werror -O2 -g \
	-I$(UDX_DIR)/include -DUDX_HOST_LIBC=1

UDX_LIB := $(UDX_DIR)/build/libudx.a
BIN     := $(BUILD)/xhci_udx

.PHONY: all clean libudx

all: $(BIN)

libudx:
	$(MAKE) -C $(UDX_DIR) lib

$(UDX_LIB):
	$(MAKE) -C $(UDX_DIR) lib

$(BIN): xhci_udx.c $(UDX_LIB)
	@mkdir -p $(dir $@)
	$(CC) $(UDX_CFLAGS) -o $@ xhci_udx.c $(UDX_LIB)
	@echo "built $@  (run: ./$@)"

clean:
	rm -rf $(BUILD)
