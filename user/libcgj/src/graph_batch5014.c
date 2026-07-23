/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5014: zip low u16 lanes of two uint32_t words.
 *
 * Surface (unique symbols):
 *   uint32_t gj_lane_zip_lo_u16_u(uint32_t a, uint32_t b);
 *     - View a and b as pairs of little-endian uint16_t lanes
 *       [lo, hi]. Return the low-half zip: result = a_lo | (b_lo << 16).
 *       Matches SIMD ZIP1 / unpack-low of 16-bit elements in a 32-bit
 *       scalar register pair.
 *   uint32_t __gj_lane_zip_lo_u16_u  (alias)
 *   __libcgj_batch5014_marker = "libcgj-batch5014"
 *
 * Exclusive continuum CREATE-ONLY (5011-5020: SIMD lane scalar unique).
 * Distinct from gj_u32_zip_nibbles_u (batch4525) and
 * gj_u32_pack_u16x2 (batch1383) — unique gj_lane_zip_lo_u16_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5014_marker[] = "libcgj-batch5014";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5014_zip_lo(uint32_t u32A, uint32_t u32B)
{
	return (u32A & 0xffffu) | ((u32B & 0xffffu) << 16);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lane_zip_lo_u16_u - zip the low 16-bit lanes of a and b.
 *
 * a: source A (low 16 bits become result bits [15:0])
 * b: source B (low 16 bits become result bits [31:16])
 *
 * Returns a_lo in the low half and b_lo in the high half.
 * Self-contained; no parent wires.
 */
uint32_t
gj_lane_zip_lo_u16_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b5014_zip_lo(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lane_zip_lo_u16_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_lane_zip_lo_u16_u")));
