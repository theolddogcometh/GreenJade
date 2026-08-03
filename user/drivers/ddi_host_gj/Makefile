# SPDX-License-Identifier: MIT OR Apache-2.0
# Freestanding ddi_host_gj — NATIVE ELF via libgj + user.ld (personality_gj style).
# Dual MIT OR Apache-2.0. Soft ≠ product. No GPL.
#
# Build (from this directory):
#   make
#   → user/drivers/ddi_host_gj/build/ddi_host.elf
#
# Optional root-style output:
#   make ROOT=../../.. OUT=$(ROOT)/build/user/ddi_host.elf
#   → build/user/ddi_host.elf

ROOT     ?= $(abspath ../../..)
BUILD    ?= $(abspath build)
CC       ?= gcc
LD       ?= ld

LIBGJ_INC  := $(ROOT)/user/libgj/include
USER_LD    := $(ROOT)/user/init/user.ld
LIBGJ_A    := $(ROOT)/build/user/libgj.a
LIBGJ_OBJS := \
	$(ROOT)/build/user/libgj/src/syscall.o \
	$(ROOT)/build/user/libgj/src/string_u.o

USER_CFLAGS := -std=c11 -ffreestanding -fno-builtin -fno-stack-protector \
	-fno-pic -mno-red-zone -msse2 \
	-Wall -Wextra -Werror -I$(LIBGJ_INC) -g -O2
USER_LDFLAGS := -nostdlib -static -T $(USER_LD) -z max-page-size=0x1000

OUT      ?= $(BUILD)/ddi_host.elf
OBJ      := $(BUILD)/ddi_host_gj.o

.PHONY: all clean libgj ddi_host

all: ddi_host

ddi_host: $(OUT)
	@echo "ddi_host: $(OUT)"

# Build libgj.a if missing (same objects as root Makefile).
libgj: $(LIBGJ_A)

$(LIBGJ_A): $(LIBGJ_OBJS)
	@mkdir -p $(dir $@)
	ar rcs $@ $(LIBGJ_OBJS)
	@echo "built $@"

$(ROOT)/build/user/libgj/src/syscall.o: $(ROOT)/user/libgj/src/syscall.S
	@mkdir -p $(dir $@)
	$(CC) $(USER_CFLAGS) -c -o $@ $<

$(ROOT)/build/user/libgj/src/string_u.o: $(ROOT)/user/libgj/src/string_u.c
	@mkdir -p $(dir $@)
	$(CC) $(USER_CFLAGS) -c -o $@ $<

$(OBJ): ddi_host_gj.c $(USER_LD)
	@mkdir -p $(dir $@)
	$(CC) $(USER_CFLAGS) -c -o $@ ddi_host_gj.c

$(OUT): $(OBJ) $(LIBGJ_A) $(USER_LD)
	@mkdir -p $(dir $@)
	$(LD) $(USER_LDFLAGS) -o $@ $(OBJ) $(LIBGJ_A)
	@echo "built $@"

clean:
	rm -rf $(BUILD)
