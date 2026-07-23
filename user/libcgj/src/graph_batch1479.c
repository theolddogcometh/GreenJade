/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1479: signed 32-bit lerp with Q16.16 weight.
 *
 * Surface (unique symbols):
 *   int32_t gj_i32_lerp_q16(int32_t a, int32_t b, uint32_t t);
 *     — linear interpolate a→b with t in Q16.16 [0, 1.0].
 *       t==0 → a; t==0x10000 → b. t is clamped to [0, 1.0].
 *       Result saturates to INT32_MIN / INT32_MAX.
 *   int32_t __gj_i32_lerp_q16  (alias)
 *   __libcgj_batch1479_marker = "libcgj-batch1479"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1479_marker[] = "libcgj-batch1479";

/* Q16.16 unit (1.0). t is treated as unsigned Q16.16. */
#define B1479_Q16_ONE  ((uint32_t)0x00010000u)

/* ---- static helpers ---------------------------------------------------- */

static int32_t
s_i32_sat_i64(int64_t i64V)
{
	if (i64V > (int64_t)INT32_MAX) {
		return INT32_MAX;
	}
	if (i64V < (int64_t)INT32_MIN) {
		return INT32_MIN;
	}
	return (int32_t)i64V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i32_lerp_q16 — signed lerp of a,b with Q16.16 weight t.
 *
 * t is clamped into [0, 1.0]. Delta and mix are computed in int64_t;
 * the final value saturates into int32_t.
 */
int32_t
gj_i32_lerp_q16(int32_t i32A, int32_t i32B, uint32_t u32T)
{
	int64_t i64Delta;
	int64_t i64Mix;

	if (u32T == 0u) {
		return i32A;
	}
	if (u32T >= B1479_Q16_ONE) {
		return i32B;
	}
	i64Delta = (int64_t)i32B - (int64_t)i32A;
	i64Mix = ((i64Delta * (int64_t)u32T) >> 16);
	return s_i32_sat_i64((int64_t)i32A + i64Mix);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_i32_lerp_q16(int32_t i32A, int32_t i32B, uint32_t u32T)
    __attribute__((alias("gj_i32_lerp_q16")));
