/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch371: integer square root for uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_isqrt_u64(uint64_t n);
 *     — Floor of the non-negative real square root of n:
 *       the greatest r with r*r <= n (and r*r fitting in 64 bits).
 *       gj_isqrt_u64(0) == 0, gj_isqrt_u64(1) == 1.
 *   uint64_t __gj_isqrt_u64  (alias)
 *   __libcgj_batch371_marker = "libcgj-batch371"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * floating point, no libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch371_marker[] = "libcgj-batch371";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_isqrt_u64 — floor integer square root of n.
 *
 * Binary search over r in [0, min(n, 2^32-1)]. floor(sqrt(UINT64_MAX))
 * is 4294967295, so the candidate root always fits in 32 bits and
 * mid*mid never overflows uint64_t under that cap.
 */
uint64_t
gj_isqrt_u64(uint64_t u64N)
{
	uint64_t u64Lo;
	uint64_t u64Hi;
	uint64_t u64Mid;
	uint64_t u64Sq;

	if (u64N <= 1u) {
		return u64N;
	}

	u64Lo = 1u;
	/* Cap high end: largest r with r*r representable in 64 bits. */
	u64Hi = (u64N < 0xffffffffULL) ? u64N : 0xffffffffULL;

	while (u64Lo < u64Hi) {
		u64Mid = u64Lo + ((u64Hi - u64Lo + 1u) >> 1);
		u64Sq = u64Mid * u64Mid;
		if (u64Sq <= u64N) {
			u64Lo = u64Mid;
		} else {
			u64Hi = u64Mid - 1u;
		}
	}
	return u64Lo;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_isqrt_u64(uint64_t u64N)
    __attribute__((alias("gj_isqrt_u64")));
