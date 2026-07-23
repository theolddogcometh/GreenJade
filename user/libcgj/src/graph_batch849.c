/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch849: uint32 binomial C(n,k); overflow → 0.
 *
 * Surface (unique symbols):
 *   uint32_t gj_binom_u32(unsigned n, unsigned k);
 *     — C(n,k). k>n → 0. Overflow of result → 0. C(n,0)=C(n,n)=1.
 *   uint32_t __gj_binom_u32  (alias)
 *   __libcgj_batch849_marker = "libcgj-batch849"
 *
 * Distinct from gj_binom / gj_binom_u64 — no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch849_marker[] = "libcgj-batch849";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b849_gcd(uint32_t u32A, uint32_t u32B)
{
	while (u32B != 0u) {
		uint32_t u32T = u32A % u32B;

		u32A = u32B;
		u32B = u32T;
	}
	return u32A;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_binom_u32 — binomial coefficient C(n, k) as uint32_t.
 *
 * Multiplicative formula with gcd reduction. Overflow → 0.
 */
uint32_t
gj_binom_u32(unsigned uN, unsigned uK)
{
	uint32_t u32R;
	unsigned uI;

	if (uK > uN) {
		return 0u;
	}
	if (uK > uN - uK) {
		uK = uN - uK;
	}
	if (uK == 0u) {
		return 1u;
	}

	u32R = 1u;
	for (uI = 0u; uI < uK; uI++) {
		uint32_t u32Num = (uint32_t)uN - (uint32_t)uI;
		uint32_t u32Den = (uint32_t)uI + 1u;
		uint32_t u32G;

		u32G = b849_gcd(u32R, u32Den);
		u32R /= u32G;
		u32Den /= u32G;
		u32G = b849_gcd(u32Num, u32Den);
		u32Num /= u32G;
		u32Den /= u32G;

		if (u32Den != 1u) {
			u32R /= u32Den;
		}

		if (u32Num != 0u && u32R > 0xffffffffu / u32Num) {
			return 0u;
		}
		u32R *= u32Num;
	}
	return u32R;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_binom_u32(unsigned uN, unsigned uK)
    __attribute__((alias("gj_binom_u32")));
