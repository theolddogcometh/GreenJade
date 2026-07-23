/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7037: sched_attr combined flag set+clr apply.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sched_attr_flag_apply_7037(uint32_t cur, uint32_t set,
 *                                          uint32_t clr);
 *     - Return (cur | set) & ~clr (soft enable then clear).
 *   uint32_t __gj_sched_attr_flag_apply_7037  (alias)
 *   __libcgj_batch7037_marker = "libcgj-batch7037"
 *
 * Exclusive continuum CREATE-ONLY (7031-7040: sched_setattr policy stubs).
 * Unique gj_sched_attr_flag_apply_7037 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7037_marker[] = "libcgj-batch7037";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7037_apply(uint32_t u32Cur, uint32_t u32Set, uint32_t u32Clr)
{
	return (u32Cur | u32Set) & ~u32Clr;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sched_attr_flag_apply_7037 - soft combined flag set then clear.
 *
 * cur: soft current sched_flags bitmask
 * set: bits to enable first
 * clr: bits to clear second (wins on overlap with set)
 *
 * Returns (cur | set) & ~clr. Soft pure-data product; does not call
 * sched_setattr.
 */
uint32_t
gj_sched_attr_flag_apply_7037(uint32_t u32Cur, uint32_t u32Set,
                              uint32_t u32Clr)
{
	(void)NULL;
	return b7037_apply(u32Cur, u32Set, u32Clr);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sched_attr_flag_apply_7037(uint32_t u32Cur, uint32_t u32Set,
                                         uint32_t u32Clr)
    __attribute__((alias("gj_sched_attr_flag_apply_7037")));
