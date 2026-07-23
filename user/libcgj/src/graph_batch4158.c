/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4158: unsigned Q16.16 linear interpolate (_q16x).
 *
 * Surface (unique symbols):
 *   uint32_t gj_q16x_lerp_u(uint32_t a, uint32_t b, uint32_t t);
 *     - a + (b - a) * t >> 16. Wrap-safe via uint64 magnitude of
 *       |b-a| (no unsigned (b-a) wrap); no __int128.
 *   uint32_t __gj_q16x_lerp_u  (alias)
 *   __libcgj_batch4158_marker = "libcgj-batch4158"
 *
 * Exclusive continuum CREATE-ONLY (4151-4160). Distinct from
 * gj_u32_lerp_q16 (batch1478) and gj_i32_lerp_q16 (batch1479) —
 * unique gj_q16x_lerp_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4158_marker[] = "libcgj-batch4158";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b4158_lerp — a + (b-a)*t >> 16 using uint64 for the |delta|*t product.
 *
 * When b >= a:  a + ((b-a)*t >> 16)
 * When a >  b:  a - ((a-b)*t >> 16)
 * Both branches avoid unsigned wrap of (b - a). uint32 factors into
 * uint64 product always fit in 64 bits ((2^32-1)^2 < 2^64).
 */
static uint32_t
b4158_lerp(uint32_t u32A, uint32_t u32B, uint32_t u32T)
{
	uint64_t u64Delta;
	uint64_t u64Adj;

	if (u32B >= u32A) {
		u64Delta = (uint64_t)u32B - (uint64_t)u32A;
		u64Adj = (u64Delta * (uint64_t)u32T) >> 16;
		return (uint32_t)((uint64_t)u32A + u64Adj);
	}
	u64Delta = (uint64_t)u32A - (uint64_t)u32B;
	u64Adj = (u64Delta * (uint64_t)u32T) >> 16;
	return (uint32_t)((uint64_t)u32A - u64Adj);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_q16x_lerp_u - unsigned Q16.16 lerp of a toward b by weight t.
 *
 * a, b: endpoints (Q16.16 or plain u32 bit patterns)
 * t:    Q16.16 weight (1.0 == 0x10000 yields b when representable)
 *
 * Returns a + (b - a) * t >> 16 with wrap-safe delta via uint64.
 * No parent wires.
 */
uint32_t
gj_q16x_lerp_u(uint32_t u32A, uint32_t u32B, uint32_t u32T)
{
	(void)NULL;
	return b4158_lerp(u32A, u32B, u32T);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_q16x_lerp_u(uint32_t u32A, uint32_t u32B, uint32_t u32T)
    __attribute__((alias("gj_q16x_lerp_u")));
