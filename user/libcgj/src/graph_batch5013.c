/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5013: byte-lane shuffle of a uint32_t (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_lane_shuffle_u32_u(uint32_t x,
 *                                  uint32_t s0, uint32_t s1,
 *                                  uint32_t s2, uint32_t s3);
 *     - Build a new 4-byte word by picking source byte lanes of x.
 *       Result lane k comes from source lane (sk & 3), for k in 0..3.
 *       Lanes are little-endian byte indices (lane 0 = bits [7:0]).
 *   uint32_t __gj_lane_shuffle_u32_u  (alias)
 *   __libcgj_batch5013_marker = "libcgj-batch5013"
 *
 * Exclusive continuum CREATE-ONLY (5011-5020: SIMD lane scalar unique).
 * Distinct from gj_shuffle_u32 (batch212, Fisher–Yates array) — unique
 * gj_lane_shuffle_u32_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5013_marker[] = "libcgj-batch5013";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5013_pick(uint32_t u32X, uint32_t u32Sel)
{
	return (u32X >> ((u32Sel & 3u) * 8u)) & 0xffu;
}

static uint32_t
b5013_shuffle(uint32_t u32X,
    uint32_t u32S0, uint32_t u32S1, uint32_t u32S2, uint32_t u32S3)
{
	return b5013_pick(u32X, u32S0)
	    | (b5013_pick(u32X, u32S1) << 8)
	    | (b5013_pick(u32X, u32S2) << 16)
	    | (b5013_pick(u32X, u32S3) << 24);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lane_shuffle_u32_u - permute byte lanes of x by four selectors.
 *
 * x:  source word
 * s0: selector for result lane 0 (modulo 4)
 * s1: selector for result lane 1 (modulo 4)
 * s2: selector for result lane 2 (modulo 4)
 * s3: selector for result lane 3 (modulo 4)
 *
 * Returns the shuffled word. Self-contained; no parent wires.
 */
uint32_t
gj_lane_shuffle_u32_u(uint32_t u32X,
    uint32_t u32S0, uint32_t u32S1, uint32_t u32S2, uint32_t u32S3)
{
	(void)NULL;
	return b5013_shuffle(u32X, u32S0, u32S1, u32S2, u32S3);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lane_shuffle_u32_u(uint32_t u32X,
    uint32_t u32S0, uint32_t u32S1, uint32_t u32S2, uint32_t u32S3)
    __attribute__((alias("gj_lane_shuffle_u32_u")));
