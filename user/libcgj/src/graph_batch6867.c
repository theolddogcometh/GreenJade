/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6867: mount_attr combined set+clr apply.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mount_attr_apply_6867(uint32_t cur, uint32_t set,
 *                                     uint32_t clr);
 *     - Return (cur | set) & ~clr (soft attr_set then attr_clr).
 *   uint32_t __gj_mount_attr_apply_6867  (alias)
 *   __libcgj_batch6867_marker = "libcgj-batch6867"
 *
 * Exclusive continuum CREATE-ONLY (6861-6870: mount_setattr attr stubs).
 * Unique gj_mount_attr_apply_6867 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6867_marker[] = "libcgj-batch6867";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6867_apply(uint32_t u32Cur, uint32_t u32Set, uint32_t u32Clr)
{
	return (u32Cur | u32Set) & ~u32Clr;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mount_attr_apply_6867 - soft combined attr_set then attr_clr.
 *
 * cur: soft current mount attr bitmask
 * set: bits to enable first
 * clr: bits to clear second (wins on overlap with set)
 *
 * Returns (cur | set) & ~clr. Soft pure-data product of the two
 * mount_attr fields; does not call mount_setattr.
 */
uint32_t
gj_mount_attr_apply_6867(uint32_t u32Cur, uint32_t u32Set, uint32_t u32Clr)
{
	(void)NULL;
	return b6867_apply(u32Cur, u32Set, u32Clr);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mount_attr_apply_6867(uint32_t u32Cur, uint32_t u32Set,
                                    uint32_t u32Clr)
    __attribute__((alias("gj_mount_attr_apply_6867")));
