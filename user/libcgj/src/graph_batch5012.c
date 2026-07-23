/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5012: insert a byte into lane i of a uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_lane_insert_u32_u(uint32_t x, uint32_t i, uint32_t b);
 *     - Replace byte lane i of x with the low 8 bits of b. Lanes are
 *       little-endian byte indices 0..3 (lane 0 = bits [7:0]). The lane
 *       index is taken modulo 4 (i & 3). Other lanes of x are preserved.
 *   uint32_t __gj_lane_insert_u32_u  (alias)
 *   __libcgj_batch5012_marker = "libcgj-batch5012"
 *
 * Exclusive continuum CREATE-ONLY (5011-5020: SIMD lane scalar unique).
 * Distinct from gj_lane_extract_u32_u (batch5011) — unique
 * gj_lane_insert_u32_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5012_marker[] = "libcgj-batch5012";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5012_insert(uint32_t u32X, uint32_t u32I, uint32_t u32B)
{
	uint32_t u32Lane;
	uint32_t u32Shift;
	uint32_t u32Mask;

	u32Lane = u32I & 3u;
	u32Shift = u32Lane * 8u;
	u32Mask = 0xffu << u32Shift;
	return (u32X & ~u32Mask) | ((u32B & 0xffu) << u32Shift);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lane_insert_u32_u - insert low 8 bits of b into byte lane i of x.
 *
 * x: source word (4 little-endian byte lanes)
 * i: lane index (modulo 4)
 * b: byte value (low 8 bits used)
 *
 * Returns the word with the selected lane replaced.
 * Self-contained; no parent wires.
 */
uint32_t
gj_lane_insert_u32_u(uint32_t u32X, uint32_t u32I, uint32_t u32B)
{
	(void)NULL;
	return b5012_insert(u32X, u32I, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lane_insert_u32_u(uint32_t u32X, uint32_t u32I, uint32_t u32B)
    __attribute__((alias("gj_lane_insert_u32_u")));
