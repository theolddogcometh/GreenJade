/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * libgj-so.so.1 — product DT_NEEDED shared object with SysV DT_HASH.
 *
 * Clean-room freestanding SO (no libc). Built -shared -fPIC with
 * --hash-style=sysv so ld-gj multi-SO resolve smokes exercise classic
 * ELF hash buckets (not GNU hash — that is libgj-gnu.so.1).
 *
 * Staged to rootfs/ESP as /lib/libgj-so.so.1 (and /usr/lib). Smoke gates
 * require a real ELF with dynamic export gj_so_export (value 0x42).
 *
 * Pure C11. Dual-licensed MIT OR Apache-2.0.
 *
 * Exports
 *   gj_so_export  — defined u64; GLOB_DAT / JUMP_SLOT targets resolve here
 *   gj_so_init    — optional ctor-style reset of the export
 */
#include <stdint.h>

/* Defined export — GLOB_DAT / JUMP_SLOT resolve to this VA (+load bias). */
volatile uint64_t gj_so_export = 0x42;

void
gj_so_init(void)
{
    gj_so_export = 0x42;
}
