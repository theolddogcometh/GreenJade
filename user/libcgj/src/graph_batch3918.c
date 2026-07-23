/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3918: divisor-count tau(n) for uint32 (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_tau_count_u(uint32_t n);
 *     - Number of positive divisors of n (d(n) / tau(n)).
 *       tau(0)=0, tau(1)=1. Multiplicative: if n = p1^e1 * ... then
 *       tau = product (e_i + 1).
 *   uint32_t __gj_u32_tau_count_u  (alias)
 *   __libcgj_batch3918_marker = "libcgj-batch3918"
 *
 * Exclusive continuum CREATE-ONLY (3911-3920). Unique
 * gj_u32_tau_count_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3918_marker[] = "libcgj-batch3918";

/* ---- freestanding helpers ---------------------------------------------- */

/* tau(n) = product over primes (e + 1) for n = product p^e. */
static uint32_t
b3918_tau(uint32_t u32N)
{
	uint32_t u32Result;
	uint32_t u32X;
	uint32_t u32P;
	uint32_t u32Exp;

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
		u32Exp = 0u;
		while ((u32X & 1u) == 0u) {
			u32X >>= 1u;
			u32Exp += 1u;
		}
		u32Result *= (u32Exp + 1u);
	}

	/* Odd factors. */
	for (u32P = 3u; u32P <= u32X / u32P; u32P += 2u) {
		if ((u32X % u32P) == 0u) {
			u32Exp = 0u;
			while ((u32X % u32P) == 0u) {
				u32X /= u32P;
				u32Exp += 1u;
			}
			u32Result *= (u32Exp + 1u);
		}
	}

	/* Remaining prime p^1 contributes (1+1)=2. */
	if (u32X > 1u) {
		u32Result *= 2u;
	}

	return u32Result;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_tau_count_u - number of positive divisors of n.
 *
 * n: input (0 -> 0)
 * No parent wires.
 */
uint32_t
gj_u32_tau_count_u(uint32_t u32N)
{
	(void)NULL;
	return b3918_tau(u32N);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_tau_count_u(uint32_t u32N)
    __attribute__((alias("gj_u32_tau_count_u")));
