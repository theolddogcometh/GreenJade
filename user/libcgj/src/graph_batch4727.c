/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4727: floor integer sqrt for uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_isqrt_u32_floor_u(uint32_t x);
 *     - Greatest r with r*r <= x. gj_isqrt_u32_floor_u(0)==0,
 *       gj_isqrt_u32_floor_u(1)==1, gj_isqrt_u32_floor_u(0xffffffffu)==65535.
 *   uint32_t __gj_isqrt_u32_floor_u  (alias)
 *   __libcgj_batch4727_marker = "libcgj-batch4727"
 *
 * Exclusive continuum CREATE-ONLY (4721-4730). Planned gj_isqrt_u32_u
 * is owned by batch2781; rename on collision to gj_isqrt_u32_floor_u.
 * Unique surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4727_marker[] = "libcgj-batch4727";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Floor isqrt via binary search. Root always fits in 16 bits so
 * mid*mid never overflows uint32_t under the hi cap of 0xffff.
 */
static uint32_t
b4727_isqrt(uint32_t u32X)
{
	uint32_t u32Lo;
	uint32_t u32Hi;
	uint32_t u32Mid;
	uint32_t u32Sq;

	if (u32X <= 1u) {
		return u32X;
	}

	u32Lo = 1u;
	u32Hi = (u32X < 0xffffu) ? u32X : 0xffffu;

	while (u32Lo < u32Hi) {
		u32Mid = u32Lo + ((u32Hi - u32Lo + 1u) >> 1);
		u32Sq = u32Mid * u32Mid;
		if (u32Sq <= u32X) {
			u32Lo = u32Mid;
		} else {
			u32Hi = u32Mid - 1u;
		}
	}
	return u32Lo;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_isqrt_u32_floor_u - floor integer square root of x (uint32_t).
 *
 * x: non-negative integer (full uint32_t domain)
 *
 * Returns the greatest r with r*r <= x. Distinct from gj_isqrt_u32_u
 * (batch2781) by unique name only. No parent wires. No __int128.
 */
uint32_t
gj_isqrt_u32_floor_u(uint32_t u32X)
{
	(void)NULL;
	return b4727_isqrt(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_isqrt_u32_floor_u(uint32_t u32X)
    __attribute__((alias("gj_isqrt_u32_floor_u")));
