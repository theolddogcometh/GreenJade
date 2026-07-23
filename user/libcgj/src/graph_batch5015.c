/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5015: zip high u16 lanes of two uint32_t words.
 *
 * Surface (unique symbols):
 *   uint32_t gj_lane_zip_hi_u16_u(uint32_t a, uint32_t b);
 *     - View a and b as pairs of little-endian uint16_t lanes
 *       [lo, hi]. Return the high-half zip:
 *         result = a_hi | (b_hi << 16)
 *       where a_hi = (a >> 16) & 0xffff and b_hi likewise.
 *       Matches SIMD ZIP2 / unpack-high of 16-bit elements in a 32-bit
 *       scalar register pair.
 *   uint32_t __gj_lane_zip_hi_u16_u  (alias)
 *   __libcgj_batch5015_marker = "libcgj-batch5015"
 *
 * Exclusive continuum CREATE-ONLY (5011-5020: SIMD lane scalar unique).
 * Pair of gj_lane_zip_lo_u16_u (batch5014). Distinct from
 * gj_u32_unzip_hi_u (batch4527) — unique gj_lane_zip_hi_u16_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5015_marker[] = "libcgj-batch5015";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5015_zip_hi(uint32_t u32A, uint32_t u32B)
{
	return ((u32A >> 16) & 0xffffu) | (u32B & 0xffff0000u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lane_zip_hi_u16_u - zip the high 16-bit lanes of a and b.
 *
 * a: source A (high 16 bits become result bits [15:0])
 * b: source B (high 16 bits become result bits [31:16])
 *
 * Returns a_hi in the low half and b_hi in the high half.
 * Self-contained; no parent wires.
 */
uint32_t
gj_lane_zip_hi_u16_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b5015_zip_hi(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lane_zip_hi_u16_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_lane_zip_hi_u16_u")));
