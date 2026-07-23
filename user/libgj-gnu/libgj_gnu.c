/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * libgj-gnu.so.1 — product DT_NEEDED SO with DT_GNU_HASH.
 *
 * Clean-room freestanding C. Counterpart to libgj-so.so.1 (SysV DT_HASH).
 * Used by ld-gj multi-SO resolve smokes and install rootfs/ESP staging.
 *
 * Build: make libgj-gnu  →  build/user/libgj-gnu.so.1
 *   -shared -nostdlib -fPIC -ffreestanding --hash-style=gnu
 *   -soname libgj-gnu.so.1
 *
 * Pure C11. Dual-licensed MIT OR Apache-2.0.
 *
 * Export surface (keep minimal until GNU-extension batches wire in):
 *   gj_gnu_export — data (0x43); GLOB_DAT / JUMP_SLOT resolve target
 *   gj_gnu_init   — function; re-stores the export value
 */
#include <stdint.h>

/* Defined export — GLOB_DAT / JUMP_SLOT targets resolve to this VA (+bias). */
volatile uint64_t gj_gnu_export = 0x43;

void
gj_gnu_init(void)
{
    gj_gnu_export = 0x43;
}
