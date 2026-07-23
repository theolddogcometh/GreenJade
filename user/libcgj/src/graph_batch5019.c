/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5019: 4-way per-lane uint8 maximum.
 *
 * Surface (unique symbols):
 *   uint32_t gj_lane_max_u8x4_u(uint32_t a, uint32_t b);
 *     - Treat a and b as four parallel little-endian uint8_t lanes.
 *       For each lane, take the unsigned maximum of the two byte
 *       values (SIMD PMAXUB-style scalar).
 *   uint32_t __gj_lane_max_u8x4_u  (alias)
 *   __libcgj_batch5019_marker = "libcgj-batch5019"
 *
 * Exclusive continuum CREATE-ONLY (5011-5020: SIMD lane scalar unique).
 * Distinct from gj_buf_max_u8_u (batch3189, buffer scan) — unique
 * gj_lane_max_u8x4_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5019_marker[] = "libcgj-batch5019";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5019_max1(uint32_t u32A, uint32_t u32B)
{
	uint32_t u32Aa;
	uint32_t u32Bb;

	u32Aa = u32A & 0xffu;
	u32Bb = u32B & 0xffu;
	if (u32Aa > u32Bb) {
		return u32Aa;
	}
	return u32Bb;
}

static uint32_t
b5019_max_x4(uint32_t u32A, uint32_t u32B)
{
	return b5019_max1(u32A, u32B)
	    | (b5019_max1(u32A >> 8, u32B >> 8) << 8)
	    | (b5019_max1(u32A >> 16, u32B >> 16) << 16)
	    | (b5019_max1(u32A >> 24, u32B >> 24) << 24);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lane_max_u8x4_u - four parallel per-byte unsigned maxima.
 *
 * a: left operand (4 byte lanes)
 * b: right operand (4 byte lanes)
 *
 * Each result lane is max(a_lane, b_lane). Self-contained; no parent
 * wires.
 */
uint32_t
gj_lane_max_u8x4_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b5019_max_x4(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lane_max_u8x4_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_lane_max_u8x4_u")));
