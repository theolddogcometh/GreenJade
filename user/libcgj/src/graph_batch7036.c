/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7036: sched_attr flag clr soft apply.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sched_attr_flag_clr_7036(uint32_t cur, uint32_t clr);
 *     - Return cur & ~clr (soft clear listed sched_flags bits).
 *   uint32_t __gj_sched_attr_flag_clr_7036  (alias)
 *   __libcgj_batch7036_marker = "libcgj-batch7036"
 *
 * Exclusive continuum CREATE-ONLY (7031-7040: sched_setattr policy stubs).
 * Unique gj_sched_attr_flag_clr_7036 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7036_marker[] = "libcgj-batch7036";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7036_clr(uint32_t u32Cur, uint32_t u32Clr)
{
	return u32Cur & ~u32Clr;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sched_attr_flag_clr_7036 - soft sched_flags clear bits.
 *
 * cur: soft current sched_flags bitmask
 * clr: bits to clear
 *
 * Returns cur & ~clr. Soft pure-data op; does not call sched_setattr.
 */
uint32_t
gj_sched_attr_flag_clr_7036(uint32_t u32Cur, uint32_t u32Clr)
{
	(void)NULL;
	return b7036_clr(u32Cur, u32Clr);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sched_attr_flag_clr_7036(uint32_t u32Cur, uint32_t u32Clr)
    __attribute__((alias("gj_sched_attr_flag_clr_7036")));
