/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4207: previous (floor) power of two for
 * uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_prev_pow2_u(uint32_t x);
 *     - Largest power of two <= x.
 *       x == 0 → 0.
 *       Already pow2 → x. 3 → 2; 7 → 4; 0xffffffff → 0x80000000.
 *   uint32_t __gj_u32_prev_pow2_u  (alias)
 *   __libcgj_batch4207_marker = "libcgj-batch4207"
 *
 * Exclusive continuum CREATE-ONLY (4201-4210). Distinct from
 * gj_u32_prev_pow2 (batch2247) / gj_prev_power2 (batch525 u64) —
 * unique gj_u32_prev_pow2_u continuum surface; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4207_marker[] = "libcgj-batch4207";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Floor x to nearest power of two not exceeding x. x must be non-zero.
 * Smear set bits downward, then isolate highest bit via x - (x >> 1).
 */
static uint32_t
b4207_prev_pow2(uint32_t u32X)
{
	u32X |= u32X >> 1;
	u32X |= u32X >> 2;
	u32X |= u32X >> 4;
	u32X |= u32X >> 8;
	u32X |= u32X >> 16;
	return u32X - (u32X >> 1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_prev_pow2_u - floor of x to the nearest (not exceeding) power
 * of two.
 *
 * Zero is defined as 0. Otherwise smear set bits downward through all
 * lower bit positions, then isolate the highest remaining bit.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_prev_pow2_u(uint32_t u32X)
{
	(void)NULL;
	if (u32X == 0u) {
		return 0u;
	}
	return b4207_prev_pow2(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_prev_pow2_u(uint32_t u32X)
    __attribute__((alias("gj_u32_prev_pow2_u")));
