/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4258: isolate lowest set bit (u32).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_isolate_lsb_u2(uint32_t x);
 *     - Return x & -x: the single lowest set bit of x, or 0 if x == 0.
 *       Pure integer two's-complement isolate; no builtins.
 *   uint32_t __gj_u32_isolate_lsb_u2  (alias)
 *   __libcgj_batch4258_marker = "libcgj-batch4258"
 *
 * Exclusive continuum CREATE-ONLY (4251-4260). Unique
 * gj_u32_isolate_lsb_u2 surface only; no multi-def. Distinct from
 * gj_isolate_lsb (batch420) and gj_isolate_lsb_u64 (batch599). No
 * parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4258_marker[] = "libcgj-batch4258";

/* ---- freestanding helpers ---------------------------------------------- */

/* Two's-complement identity: x & -x (unsigned wrap of -x is defined). */
static uint32_t
b4258_isolate_lsb(uint32_t u32X)
{
	return u32X & (uint32_t)(0u - u32X);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_isolate_lsb_u2 - keep only the least-significant 1-bit of x.
 *
 * x: value to isolate
 *
 * Examples: 0 -> 0; 0x10 -> 0x10; 0x12 -> 0x2; 0xffffffff -> 1.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_u32_isolate_lsb_u2(uint32_t u32X)
{
	(void)NULL;
	return b4258_isolate_lsb(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_isolate_lsb_u2(uint32_t u32X)
    __attribute__((alias("gj_u32_isolate_lsb_u2")));
