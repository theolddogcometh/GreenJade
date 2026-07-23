/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4728: floor integer sqrt for uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_isqrt_u64_floor_u(uint64_t x);
 *     - Greatest r with r*r <= x. gj_isqrt_u64_floor_u(0)==0,
 *       gj_isqrt_u64_floor_u(1)==1; root always fits in 32 bits.
 *   uint64_t __gj_isqrt_u64_floor_u  (alias)
 *   __libcgj_batch4728_marker = "libcgj-batch4728"
 *
 * Exclusive continuum CREATE-ONLY (4721-4730). Planned gj_isqrt_u64_u
 * is owned by batch2782; rename on collision to gj_isqrt_u64_floor_u.
 * Distinct from gj_isqrt_u64 (batch371). Unique surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4728_marker[] = "libcgj-batch4728";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Floor isqrt via binary search. floor(sqrt(UINT64_MAX)) is 2^32-1 so
 * mid*mid stays in uint64_t under the hi cap of 0xffffffff.
 */
static uint64_t
b4728_isqrt(uint64_t u64X)
{
	uint64_t u64Lo;
	uint64_t u64Hi;
	uint64_t u64Mid;
	uint64_t u64Sq;

	if (u64X <= 1ull) {
		return u64X;
	}

	u64Lo = 1ull;
	u64Hi = (u64X < 0xffffffffull) ? u64X : 0xffffffffull;

	while (u64Lo < u64Hi) {
		u64Mid = u64Lo + ((u64Hi - u64Lo + 1ull) >> 1);
		u64Sq = u64Mid * u64Mid;
		if (u64Sq <= u64X) {
			u64Lo = u64Mid;
		} else {
			u64Hi = u64Mid - 1ull;
		}
	}
	return u64Lo;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_isqrt_u64_floor_u - floor integer square root of x (uint64_t).
 *
 * x: non-negative integer (full uint64_t domain)
 *
 * Returns the greatest r with r*r <= x. Distinct from gj_isqrt_u64_u
 * (batch2782) / gj_isqrt_u64 (batch371). No parent wires. No __int128.
 */
uint64_t
gj_isqrt_u64_floor_u(uint64_t u64X)
{
	(void)NULL;
	return b4728_isqrt(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_isqrt_u64_floor_u(uint64_t u64X)
    __attribute__((alias("gj_isqrt_u64_floor_u")));
