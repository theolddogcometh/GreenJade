/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3916: Euler totient phi(n) for uint32 (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_phi_euler_u(uint32_t n);
 *     - Euler's totient: count of k in 1..n with gcd(k,n)==1.
 *       phi(0)=0, phi(1)=1. Factorization via trial division; for each
 *       distinct prime p | n: result -= result/p.
 *   uint32_t __gj_u32_phi_euler_u  (alias)
 *   __libcgj_batch3916_marker = "libcgj-batch3916"
 *
 * Exclusive continuum CREATE-ONLY (3911-3920). Unique
 * gj_u32_phi_euler_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3916_marker[] = "libcgj-batch3916";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Euler totient via prime factorization:
 *   phi(n) = n * product_{p|n} (1 - 1/p)
 * Implemented as successive result -= result/p for each distinct p.
 */
static uint32_t
b3916_phi(uint32_t u32N)
{
	uint32_t u32Result;
	uint32_t u32X;
	uint32_t u32P;

	if (u32N == 0u) {
		return 0u;
	}
	if (u32N == 1u) {
		return 1u;
	}

	u32Result = u32N;
	u32X = u32N;

	/* Factor out 2. */
	if ((u32X & 1u) == 0u) {
		while ((u32X & 1u) == 0u) {
			u32X >>= 1u;
		}
		u32Result -= u32Result / 2u;
	}

	/* Odd factors from 3 up to sqrt(x). */
	for (u32P = 3u; u32P <= u32X / u32P; u32P += 2u) {
		if ((u32X % u32P) == 0u) {
			while ((u32X % u32P) == 0u) {
				u32X /= u32P;
			}
			u32Result -= u32Result / u32P;
		}
	}

	/* Remaining prime factor > 1. */
	if (u32X > 1u) {
		u32Result -= u32Result / u32X;
	}

	return u32Result;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_phi_euler_u - Euler's totient function phi(n).
 *
 * n: input (0 -> 0)
 * No parent wires.
 */
uint32_t
gj_u32_phi_euler_u(uint32_t u32N)
{
	(void)NULL;
	return b3916_phi(u32N);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_phi_euler_u(uint32_t u32N)
    __attribute__((alias("gj_u32_phi_euler_u")));
