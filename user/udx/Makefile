# SPDX-License-Identifier: MIT OR Apache-2.0
# Local UDX build (optional). Prefer repo-root: make udx / make udx-example.
# Does not replace or modify the top-level Makefile.

ROOT     := $(abspath ../..)
BUILD    ?= $(abspath build)
CC       ?= gcc

UDX_CFLAGS := -std=c11 -Wall -Wextra -Werror -Iinclude -Isrc -O2 -g -DUDX_HOST_LIBC=1

UDX_SRC := \
	src/core.c \
	src/host.c \
	src/pci.c \
	src/irq.c \
	src/dma.c \
	src/mmio.c \
	src/work.c

UDX_OBJS := $(patsubst %.c,$(BUILD)/%.o,$(UDX_SRC))
UDX_LIB  := $(BUILD)/libudx.a
UDX_EXAMPLE := $(BUILD)/udx_skeleton

.PHONY: all lib example clean

all: lib example

lib: $(UDX_LIB)

example: $(UDX_EXAMPLE)

$(UDX_LIB): $(UDX_OBJS)
	@mkdir -p $(dir $@)
	ar rcs $@ $(UDX_OBJS)
	@echo "built $@"

$(UDX_EXAMPLE): examples/skeleton_pci_driver.c $(UDX_LIB)
	@mkdir -p $(dir $@)
	$(CC) $(UDX_CFLAGS) -o $@ examples/skeleton_pci_driver.c $(UDX_LIB)
	@echo "built $@  (run: ./$@)"

$(BUILD)/src/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(UDX_CFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILD)
