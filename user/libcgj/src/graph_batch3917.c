/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3917: divisor-sum sigma(n) for uint32 (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_sigma_sum_u(uint32_t n);
 *     - Sum of positive divisors of n (sigma_1 / sigma).
 *       sigma(0)=0, sigma(1)=1. Multiplicative form via factorization:
 *       if n = p1^e1 * ... then sigma = product (p^{e+1}-1)/(p-1).
 *   uint32_t __gj_u32_sigma_sum_u  (alias)
 *   __libcgj_batch3917_marker = "libcgj-batch3917"
 *
 * Exclusive continuum CREATE-ONLY (3911-3920). Unique
 * gj_u32_sigma_sum_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3917_marker[] = "libcgj-batch3917";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * sigma(n) via prime factorization. For each p^e || n, multiply by
 * 1 + p + p^2 + ... + p^e = (p^{e+1} - 1) / (p - 1).
 * Geometric sum accumulated iteratively to avoid large intermediates.
 */
static uint32_t
b3917_sigma(uint32_t u32N)
{
	uint32_t u32Result;
	uint32_t u32X;
	uint32_t u32P;
	uint32_t u32Term;
	uint32_t u32Geom;

	if (u32N == 0u) {
		return 0u;
	}
	if (u32N == 1u) {
		return 1u;
	}

	u32Result = 1u;
	u32X = u32N;

	/* Factor 2. */
	if ((u32X & 1u) == 0u) {
		u32Term = 1u;
		u32Geom = 1u;
		while ((u32X & 1u) == 0u) {
			u32X >>= 1u;
			u32Term <<= 1u;
			u32Geom += u32Term;
		}
		u32Result *= u32Geom;
	}

	/* Odd factors. */
	for (u32P = 3u; u32P <= u32X / u32P; u32P += 2u) {
		if ((u32X % u32P) == 0u) {
			u32Term = 1u;
			u32Geom = 1u;
			while ((u32X % u32P) == 0u) {
				u32X /= u32P;
				u32Term *= u32P;
				u32Geom += u32Term;
			}
			u32Result *= u32Geom;
		}
	}

	/* Remaining prime p^1: sigma contrib = 1 + p. */
	if (u32X > 1u) {
		u32Result *= (1u + u32X);
	}

	return u32Result;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_sigma_sum_u - sum of positive divisors of n.
 *
 * n: input (0 -> 0)
 * No parent wires.
 */
uint32_t
gj_u32_sigma_sum_u(uint32_t u32N)
{
	(void)NULL;
	return b3917_sigma(u32N);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_sigma_sum_u(uint32_t u32N)
    __attribute__((alias("gj_u32_sigma_sum_u")));
