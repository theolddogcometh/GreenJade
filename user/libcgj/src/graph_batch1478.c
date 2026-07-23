/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1478: unsigned 32-bit lerp with Q16.16 weight.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_lerp_q16(uint32_t a, uint32_t b, uint32_t t);
 *     — linear interpolate a→b with t in Q16.16 [0, 1.0].
 *       t==0 → a; t==0x10000 → b. t is clamped to [0, 1.0].
 *   uint32_t __gj_u32_lerp_q16  (alias)
 *   __libcgj_batch1478_marker = "libcgj-batch1478"
 *
 * Formula: a + ((int64)(b - a) * t) >> 16, computed without signed
 * overflow of the delta by promoting both endpoints to int64.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1478_marker[] = "libcgj-batch1478";

/* Q16.16 unit (1.0). t is treated as unsigned Q16.16. */
#define B1478_Q16_ONE  ((uint32_t)0x00010000u)

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_lerp_q16 — unsigned lerp of a,b with Q16.16 weight t.
 *
 * t is clamped into [0, 1.0]. Uses signed int64 delta so a>b works
 * without unsigned wrap of (b - a).
 */
uint32_t
gj_u32_lerp_q16(uint32_t u32A, uint32_t u32B, uint32_t u32T)
{
	int64_t i64Delta;
	int64_t i64Mix;

	if (u32T == 0u) {
		return u32A;
	}
	if (u32T >= B1478_Q16_ONE) {
		return u32B;
	}
	i64Delta = (int64_t)u32B - (int64_t)u32A;
	i64Mix = ((i64Delta * (int64_t)u32T) >> 16);
	return (uint32_t)((int64_t)u32A + i64Mix);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_lerp_q16(uint32_t u32A, uint32_t u32B, uint32_t u32T)
    __attribute__((alias("gj_u32_lerp_q16")));
