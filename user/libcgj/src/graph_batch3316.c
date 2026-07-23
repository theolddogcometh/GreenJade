/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3316: Jacobi symbol on uint32_t.
 *
 * Surface (unique symbols):
 *   int gj_u32_jacobi_u(uint32_t a, uint32_t n);
 *     — Jacobi symbol (a/n). Requires n odd and n > 0.
 *       Returns -1, 0, or +1. n even or n == 0 → 0 (soft invalid).
 *       a is reduced mod n internally.
 *   int __gj_u32_jacobi_u  (alias)
 *   __libcgj_batch3316_marker = "libcgj-batch3316"
 *
 * CREATE-ONLY exclusive continuum wave (3311–3320). Unique
 * gj_u32_jacobi_u surface only; no multi-def. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3316_marker[] = "libcgj-batch3316";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Classical binary Jacobi algorithm.
 * Preconditions: n odd, n > 0. a may be any uint32 (reduced mod n).
 * Returns -1, 0, or +1.
 */
static int
b3316_jacobi(uint32_t u32A, uint32_t u32N)
{
	int iResult = 1;
	uint32_t u32Tmp;

	if ((u32N & 1u) == 0u || u32N == 0u) {
		return 0;
	}

	u32A %= u32N;
	while (u32A != 0u) {
		/* Factor out powers of 2 from a. */
		while ((u32A & 1u) == 0u) {
			u32A >>= 1;
			/* (2/n) = (-1)^((n^2-1)/8); n%8 in {3,5} → -1. */
			u32Tmp = u32N & 7u;
			if (u32Tmp == 3u || u32Tmp == 5u) {
				iResult = -iResult;
			}
		}
		/* Quadratic reciprocity: swap a, n when both ≡ 3 (mod 4). */
		if ((u32A & 3u) == 3u && (u32N & 3u) == 3u) {
			iResult = -iResult;
		}
		/* Swap a <-> n (a is odd here). */
		u32Tmp = u32A;
		u32A = u32N % u32Tmp;
		u32N = u32Tmp;
	}

	return (u32N == 1u) ? iResult : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_jacobi_u — Jacobi symbol (a/n).
 *
 * a: numerator (reduced mod n)
 * n: odd positive denominator (even/zero → 0 soft invalid)
 *
 * Returns -1, 0, or +1.
 */
int
gj_u32_jacobi_u(uint32_t u32A, uint32_t u32N)
{
	(void)NULL;
	return b3316_jacobi(u32A, u32N);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_jacobi_u(uint32_t u32A, uint32_t u32N)
    __attribute__((alias("gj_u32_jacobi_u")));
