/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2781: integer square root for uint32_t (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_isqrt_u32_u(uint32_t n);
 *     - Floor of the non-negative real square root of n: the greatest
 *       r with r*r <= n. gj_isqrt_u32_u(0) == 0, gj_isqrt_u32_u(1) == 1.
 *   uint32_t __gj_isqrt_u32_u  (alias)
 *   __libcgj_batch2781_marker = "libcgj-batch2781"
 *
 * Integer-root exclusive wave (2781-2790). Distinct from gj_isqrt_u64
 * (batch371) — unique _u u32 surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2781_marker[] = "libcgj-batch2781";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Floor integer square root of u32N via binary search.
 * floor(sqrt(UINT32_MAX)) is 65535, so the candidate root always fits
 * in 16 bits and mid*mid never overflows uint32_t under that cap.
 */
static uint32_t
b2781_isqrt(uint32_t u32N)
{
	uint32_t u32Lo;
	uint32_t u32Hi;
	uint32_t u32Mid;
	uint32_t u32Sq;

	if (u32N <= 1u) {
		return u32N;
	}

	u32Lo = 1u;
	/* Cap high end: largest r with r*r representable in 32 bits. */
	u32Hi = (u32N < 0xffffu) ? u32N : 0xffffu;

	while (u32Lo < u32Hi) {
		u32Mid = u32Lo + ((u32Hi - u32Lo + 1u) >> 1);
		u32Sq = u32Mid * u32Mid;
		if (u32Sq <= u32N) {
			u32Lo = u32Mid;
		} else {
			u32Hi = u32Mid - 1u;
		}
	}
	return u32Lo;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_isqrt_u32_u - floor integer square root of n.
 *
 * n: non-negative integer (full uint32_t domain)
 *
 * Returns the greatest r with r*r <= n.
 */
uint32_t
gj_isqrt_u32_u(uint32_t u32N)
{
	(void)NULL;
	return b2781_isqrt(u32N);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_isqrt_u32_u(uint32_t u32N)
    __attribute__((alias("gj_isqrt_u32_u")));
