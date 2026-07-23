/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4019: isolate the lowest set bit of a uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_isolate_lsb_u(uint64_t x);
 *     - Return x & -x: isolate the least-significant 1-bit of x, or 0
 *       if x == 0. Pure unsigned two's-complement; no builtins.
 *   uint64_t __gj_u64_isolate_lsb_u  (alias)
 *   __libcgj_batch4019_marker = "libcgj-batch4019"
 *
 * Exclusive continuum CREATE-ONLY (4011-4020). Distinct from
 * gj_u32_isolate_lsb_u (batch4018), gj_u64_blsi_u (batch3406),
 * gj_isolate_lsb (batch420), and gj_isolate_lsb_u64 (batch599) —
 * unique gj_u64_isolate_lsb_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4019_marker[] = "libcgj-batch4019";

/* ---- freestanding helpers ---------------------------------------------- */

/* Isolate lowest set bit via x & -x. */
static uint64_t
b4019_isolate_lsb(uint64_t u64X)
{
	return u64X & (uint64_t)(0ull - u64X);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_isolate_lsb_u - isolate the least-significant 1-bit of x.
 *
 * x: unsigned 64-bit value
 *
 * Two's-complement identity: x & -x (unsigned wrap of -x is defined).
 * Examples: 0 -> 0; 0x10 -> 0x10; 0x12 -> 0x2; ~0ull -> 1.
 * No parent wires.
 */
uint64_t
gj_u64_isolate_lsb_u(uint64_t u64X)
{
	(void)NULL;
	return b4019_isolate_lsb(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_isolate_lsb_u(uint64_t u64X)
    __attribute__((alias("gj_u64_isolate_lsb_u")));
