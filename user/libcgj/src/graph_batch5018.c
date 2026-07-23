/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5018: 4-way rounded average of u8 lanes.
 *
 * Surface (unique symbols):
 *   uint32_t gj_lane_avg_u8x4_u(uint32_t a, uint32_t b);
 *     - Treat a and b as four parallel little-endian uint8_t lanes.
 *       For each lane, compute the rounded average (a + b + 1) >> 1,
 *       matching classic SIMD PAVGB semantics (no intermediate
 *       overflow beyond a 9-bit sum per lane).
 *   uint32_t __gj_lane_avg_u8x4_u  (alias)
 *   __libcgj_batch5018_marker = "libcgj-batch5018"
 *
 * Exclusive continuum CREATE-ONLY (5011-5020: SIMD lane scalar unique).
 * Distinct from gj_avg_u64 (batch381, floor whole-word average) —
 * unique gj_lane_avg_u8x4_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5018_marker[] = "libcgj-batch5018";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5018_avg1(uint32_t u32A, uint32_t u32B)
{
	/* Rounded: (a + b + 1) >> 1 over a single byte lane. */
	return ((u32A & 0xffu) + (u32B & 0xffu) + 1u) >> 1;
}

static uint32_t
b5018_avg_x4(uint32_t u32A, uint32_t u32B)
{
	return b5018_avg1(u32A, u32B)
	    | (b5018_avg1(u32A >> 8, u32B >> 8) << 8)
	    | (b5018_avg1(u32A >> 16, u32B >> 16) << 16)
	    | (b5018_avg1(u32A >> 24, u32B >> 24) << 24);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lane_avg_u8x4_u - four parallel rounded uint8 averages.
 *
 * a: left operand (4 byte lanes)
 * b: right operand (4 byte lanes)
 *
 * Each result lane is (a_lane + b_lane + 1) >> 1.
 * Self-contained; no parent wires.
 */
uint32_t
gj_lane_avg_u8x4_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b5018_avg_x4(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lane_avg_u8x4_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_lane_avg_u8x4_u")));
