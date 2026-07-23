/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5011: extract byte lane i from a uint32_t (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_lane_extract_u32_u(uint32_t x, uint32_t i);
 *     - Return the selected byte lane of x as a zero-extended uint32_t.
 *       Lanes are little-endian byte indices 0..3 (lane 0 = bits [7:0]).
 *       The lane index is taken modulo 4 (i & 3).
 *   uint32_t __gj_lane_extract_u32_u  (alias)
 *   __libcgj_batch5011_marker = "libcgj-batch5011"
 *
 * Exclusive continuum CREATE-ONLY (5011-5020: SIMD lane scalar unique —
 * lane_extract_u32_u, lane_insert_u32_u, lane_shuffle_u32_u,
 * lane_zip_lo_u16_u, lane_zip_hi_u16_u, lane_add_sat_u8x4_u,
 * lane_sub_sat_u8x4_u, lane_avg_u8x4_u, lane_max_u8x4_u,
 * batch_id_5020). Unique gj_lane_extract_u32_u surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5011_marker[] = "libcgj-batch5011";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5011_extract(uint32_t u32X, uint32_t u32I)
{
	uint32_t u32Lane;

	u32Lane = u32I & 3u;
	return (u32X >> (u32Lane * 8u)) & 0xffu;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lane_extract_u32_u - extract byte lane i from a uint32_t word.
 *
 * x: source word (4 little-endian byte lanes)
 * i: lane index (modulo 4)
 *
 * Returns the selected byte zero-extended into a uint32_t.
 * Self-contained; no parent wires.
 */
uint32_t
gj_lane_extract_u32_u(uint32_t u32X, uint32_t u32I)
{
	(void)NULL;
	return b5011_extract(u32X, u32I);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lane_extract_u32_u(uint32_t u32X, uint32_t u32I)
    __attribute__((alias("gj_lane_extract_u32_u")));
