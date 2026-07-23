/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3458: uint64_t lerp with Q16.16 weight
 * (exclusive _u2; rename on collision with batch2649).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_lerp_q16_u2(uint64_t a, uint64_t b, uint32_t t);
 *     - Linear interpolate a→b with t in Q16.16 [0, 1.0].
 *       t==0 → a; t>=0x10000 → b. t is clamped to [0, 1.0].
 *   uint64_t __gj_u64_lerp_q16_u2  (alias)
 *   __libcgj_batch3458_marker = "libcgj-batch3458"
 *
 * CREATE-ONLY exclusive continuum wave (3451-3460). Unique
 * gj_u64_lerp_q16_u2 surface only; no multi-def. Distinct from
 * gj_u64_lerp_q16_u (batch2649) and gj_u32_lerp_q16 (batch1478) /
 * gj_u32_lerp_q16_u (batch3457). No parent wires. No __int128: wide
 * product via 32-bit limbs.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3458_marker[] = "libcgj-batch3458";

/* Q16.16 unit (1.0). t is treated as unsigned Q16.16. */
#define B3458_Q16_ONE  ((uint32_t)0x00010000u)

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Floor of (x * t) / 2^16 for t in [0, 0xFFFF].
 * Splits x into 32-bit limbs so the full product never needs __int128.
 * (hi*t)<<16 + (lo*t)>>16 equals floor((x * t) / 65536) and fits uint64
 * for all x and t in the clamped range.
 */
static uint64_t
b3458_mul_shr16(uint64_t u64X, uint32_t u32T)
{
	uint64_t u64Lo;
	uint64_t u64Hi;

	u64Lo = (u64X & 0xffffffffull) * (uint64_t)u32T;
	u64Hi = (u64X >> 32) * (uint64_t)u32T;
	return (u64Hi << 16) + (u64Lo >> 16);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_lerp_q16_u2 — unsigned lerp of a,b with Q16.16 weight t.
 *
 * t is clamped into [0, 1.0]. Direction uses abs-diff so a>b works
 * without unsigned wrap of (b - a). Mix scaled without __int128.
 * Does not call libc. No parent wires.
 */
uint64_t
gj_u64_lerp_q16_u2(uint64_t u64A, uint64_t u64B, uint32_t u32T)
{
	uint64_t u64Diff;
	uint64_t u64Mix;

	(void)NULL;
	if (u32T == 0u) {
		return u64A;
	}
	if (u32T >= B3458_Q16_ONE) {
		return u64B;
	}
	if (u64B >= u64A) {
		u64Diff = u64B - u64A;
		u64Mix = b3458_mul_shr16(u64Diff, u32T);
		return u64A + u64Mix;
	}
	u64Diff = u64A - u64B;
	u64Mix = b3458_mul_shr16(u64Diff, u32T);
	return u64A - u64Mix;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_lerp_q16_u2(uint64_t u64A, uint64_t u64B, uint32_t u32T)
    __attribute__((alias("gj_u64_lerp_q16_u2")));
