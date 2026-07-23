/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4018: isolate the lowest set bit of a uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_isolate_lsb_u(uint32_t x);
 *     - Return x & -x: isolate the least-significant 1-bit of x, or 0
 *       if x == 0. Pure unsigned two's-complement; no builtins.
 *   uint32_t __gj_u32_isolate_lsb_u  (alias)
 *   __libcgj_batch4018_marker = "libcgj-batch4018"
 *
 * Exclusive continuum CREATE-ONLY (4011-4020). Distinct from
 * gj_u32_blsi_u (batch3405), gj_isolate_lsb (batch420), and
 * gj_isolate_lsb_u64 (batch599) — unique gj_u32_isolate_lsb_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4018_marker[] = "libcgj-batch4018";

/* ---- freestanding helpers ---------------------------------------------- */

/* Isolate lowest set bit via x & -x. */
static uint32_t
b4018_isolate_lsb(uint32_t u32X)
{
	return u32X & (uint32_t)(0u - u32X);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_isolate_lsb_u - isolate the least-significant 1-bit of x.
 *
 * x: unsigned 32-bit value
 *
 * Two's-complement identity: x & -x (unsigned wrap of -x is defined).
 * Examples: 0 -> 0; 0x10 -> 0x10; 0x12 -> 0x2; 0xffffffffu -> 1.
 * No parent wires.
 */
uint32_t
gj_u32_isolate_lsb_u(uint32_t u32X)
{
	(void)NULL;
	return b4018_isolate_lsb(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_isolate_lsb_u(uint32_t u32X)
    __attribute__((alias("gj_u32_isolate_lsb_u")));
