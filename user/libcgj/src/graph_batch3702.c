/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3702: ceiling integer square root for uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_sqrt_ceil_u(uint64_t x);
 *     - Smallest r such that r*r >= x (ceil of real square root).
 *       gj_u64_sqrt_ceil_u(0)==0, gj_u64_sqrt_ceil_u(1)==1,
 *       gj_u64_sqrt_ceil_u(2)==2, gj_u64_sqrt_ceil_u(4)==2.
 *       For non-perfect squares near UINT64_MAX the result may be
 *       2^32 (r*r would overflow uint64_t; still the mathematical
 *       ceiling root, returned as uint64_t).
 *   uint64_t __gj_u64_sqrt_ceil_u  (alias)
 *   __libcgj_batch3702_marker = "libcgj-batch3702"
 *
 * Exclusive continuum CREATE-ONLY (3701-3710). Distinct from
 * gj_u64_isqrt_ceil_u (batch3306) and gj_isqrt_u64 (batch371 floor) —
 * unique sqrt_ceil_u surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3702_marker[] = "libcgj-batch3702";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Floor isqrt of u64X. floor(sqrt(UINT64_MAX)) == 0xffffffff, so mid
 * always fits in 32 bits and mid*mid never overflows uint64_t.
 */
static uint64_t
b3702_isqrt_floor(uint64_t u64X)
{
	uint64_t u64Lo;
	uint64_t u64Hi;
	uint64_t u64Mid;
	uint64_t u64Sq;

	if (u64X <= 1ULL) {
		return u64X;
	}

	u64Lo = 1ULL;
	u64Hi = (u64X < 0xffffffffULL) ? u64X : 0xffffffffULL;

	while (u64Lo < u64Hi) {
		u64Mid = u64Lo + ((u64Hi - u64Lo + 1ULL) >> 1);
		u64Sq = u64Mid * u64Mid;
		if (u64Sq <= u64X) {
			u64Lo = u64Mid;
		} else {
			u64Hi = u64Mid - 1ULL;
		}
	}
	return u64Lo;
}

/* Ceiling isqrt: floor, then +1 if not a perfect square. */
static uint64_t
b3702_sqrt_ceil(uint64_t u64X)
{
	uint64_t u64R;
	uint64_t u64Sq;

	if (u64X <= 1ULL) {
		return u64X;
	}

	u64R = b3702_isqrt_floor(u64X);
	u64Sq = u64R * u64R;
	if (u64Sq == u64X) {
		return u64R;
	}
	/* Not perfect: next integer (may be 0x100000000 for UINT64_MAX). */
	return u64R + 1ULL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_sqrt_ceil_u - ceiling integer square root of x.
 *
 * x: unsigned 64-bit value
 *
 * Returns the smallest r with r*r >= x (or r == 2^32 when x is not a
 * perfect square above floor(sqrt(UINT64_MAX))^2). No parent wires.
 */
uint64_t
gj_u64_sqrt_ceil_u(uint64_t x)
{
	(void)NULL;
	return b3702_sqrt_ceil(x);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_sqrt_ceil_u(uint64_t x)
    __attribute__((alias("gj_u64_sqrt_ceil_u")));
