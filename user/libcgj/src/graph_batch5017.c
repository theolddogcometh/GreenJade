/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5017: 4-way saturating u8 sub on a uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_lane_sub_sat_u8x4_u(uint32_t a, uint32_t b);
 *     - Treat a and b as four parallel little-endian uint8_t lanes.
 *       For each lane, compute saturating unsigned subtract (clamp to
 *       0 on underflow). Independent of the other lanes.
 *   uint32_t __gj_lane_sub_sat_u8x4_u  (alias)
 *   __libcgj_batch5017_marker = "libcgj-batch5017"
 *
 * Exclusive continuum CREATE-ONLY (5011-5020: SIMD lane scalar unique).
 * Pair of gj_lane_add_sat_u8x4_u (batch5016). Distinct from
 * gj_u8_sat_sub (batch2372) and gj_u32_sat_sub (batch924) — unique
 * gj_lane_sub_sat_u8x4_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5017_marker[] = "libcgj-batch5017";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5017_sub_sat1(uint32_t u32A, uint32_t u32B)
{
	uint32_t u32Aa;
	uint32_t u32Bb;

	u32Aa = u32A & 0xffu;
	u32Bb = u32B & 0xffu;
	if (u32Aa < u32Bb) {
		return 0u;
	}
	return u32Aa - u32Bb;
}

static uint32_t
b5017_sub_sat_x4(uint32_t u32A, uint32_t u32B)
{
	return b5017_sub_sat1(u32A, u32B)
	    | (b5017_sub_sat1(u32A >> 8, u32B >> 8) << 8)
	    | (b5017_sub_sat1(u32A >> 16, u32B >> 16) << 16)
	    | (b5017_sub_sat1(u32A >> 24, u32B >> 24) << 24);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lane_sub_sat_u8x4_u - four parallel saturating uint8 subtracts.
 *
 * a: minuend (4 byte lanes)
 * b: subtrahend (4 byte lanes)
 *
 * Each result lane is max(a_lane - b_lane, 0). No wrap.
 * Self-contained; no parent wires.
 */
uint32_t
gj_lane_sub_sat_u8x4_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b5017_sub_sat_x4(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lane_sub_sat_u8x4_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_lane_sub_sat_u8x4_u")));
