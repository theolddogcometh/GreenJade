/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3457: uint32_t lerp with Q16.16 weight
 * (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_lerp_q16_u(uint32_t a, uint32_t b, uint32_t t);
 *     - Linear interpolate a→b with t in Q16.16 [0, 1.0].
 *       t==0 → a; t>=0x10000 → b. t is clamped to [0, 1.0].
 *   uint32_t __gj_u32_lerp_q16_u  (alias)
 *   __libcgj_batch3457_marker = "libcgj-batch3457"
 *
 * CREATE-ONLY exclusive continuum wave (3451-3460). Unique
 * gj_u32_lerp_q16_u surface only; no multi-def. Distinct from
 * gj_u32_lerp_q16 (batch1478) and gj_u64_lerp_q16_u (batch2649).
 * No parent wires. No __int128 (uses unsigned abs-diff mix).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3457_marker[] = "libcgj-batch3457";

/* Q16.16 unit (1.0). t is treated as unsigned Q16.16. */
#define B3457_Q16_ONE  ((uint32_t)0x00010000u)

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor of (x * t) / 2^16 for t in [0, 0xFFFF]. */
static uint32_t
b3457_mul_shr16(uint32_t u32X, uint32_t u32T)
{
	return (uint32_t)(((uint64_t)u32X * (uint64_t)u32T) >> 16);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_lerp_q16_u — unsigned lerp of a,b with Q16.16 weight t.
 *
 * t is clamped into [0, 1.0]. Direction uses abs-diff so a>b works
 * without unsigned wrap of (b - a). Mix scaled without __int128.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_u32_lerp_q16_u(uint32_t u32A, uint32_t u32B, uint32_t u32T)
{
	uint32_t u32Diff;
	uint32_t u32Mix;

	(void)NULL;
	if (u32T == 0u) {
		return u32A;
	}
	if (u32T >= B3457_Q16_ONE) {
		return u32B;
	}
	if (u32B >= u32A) {
		u32Diff = u32B - u32A;
		u32Mix = b3457_mul_shr16(u32Diff, u32T);
		return u32A + u32Mix;
	}
	u32Diff = u32A - u32B;
	u32Mix = b3457_mul_shr16(u32Diff, u32T);
	return u32A - u32Mix;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_lerp_q16_u(uint32_t u32A, uint32_t u32B, uint32_t u32T)
    __attribute__((alias("gj_u32_lerp_q16_u")));
