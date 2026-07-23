/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5016: 4-way saturating u8 add on a uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_lane_add_sat_u8x4_u(uint32_t a, uint32_t b);
 *     - Treat a and b as four parallel little-endian uint8_t lanes.
 *       For each lane, compute saturating unsigned add (clamp to 0xff
 *       on overflow). Independent of the other lanes; no cross-lane
 *       carry.
 *   uint32_t __gj_lane_add_sat_u8x4_u  (alias)
 *   __libcgj_batch5016_marker = "libcgj-batch5016"
 *
 * Exclusive continuum CREATE-ONLY (5011-5020: SIMD lane scalar unique).
 * Distinct from gj_u32_sat_add (batch923) and gj_u8_sat_mul (batch2377)
 * — unique gj_lane_add_sat_u8x4_u surface only; no multi-def. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5016_marker[] = "libcgj-batch5016";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5016_add_sat1(uint32_t u32A, uint32_t u32B)
{
	uint32_t u32S;

	u32S = (u32A & 0xffu) + (u32B & 0xffu);
	if (u32S > 0xffu) {
		return 0xffu;
	}
	return u32S;
}

static uint32_t
b5016_add_sat_x4(uint32_t u32A, uint32_t u32B)
{
	return b5016_add_sat1(u32A, u32B)
	    | (b5016_add_sat1(u32A >> 8, u32B >> 8) << 8)
	    | (b5016_add_sat1(u32A >> 16, u32B >> 16) << 16)
	    | (b5016_add_sat1(u32A >> 24, u32B >> 24) << 24);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lane_add_sat_u8x4_u - four parallel saturating uint8 adds.
 *
 * a: left operand (4 byte lanes)
 * b: right operand (4 byte lanes)
 *
 * Each result lane is min(a_lane + b_lane, 255). No wrap, no cross-lane
 * carry. Self-contained; no parent wires.
 */
uint32_t
gj_lane_add_sat_u8x4_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b5016_add_sat_x4(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lane_add_sat_u8x4_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_lane_add_sat_u8x4_u")));
