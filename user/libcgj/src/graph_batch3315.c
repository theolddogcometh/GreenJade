/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3315: two-congruence CRT on uint32_t.
 *
 * Surface (unique symbols):
 *   int gj_u32_crt_u(uint32_t a1, uint32_t m1, uint32_t a2, uint32_t m2,
 *                    uint32_t *x, uint32_t *m);
 *     — solve x ≡ a1 (mod m1), x ≡ a2 (mod m2).
 *       On success: *x in [0, *m), *m = lcm(m1, m2), return 0.
 *       On failure (NULL outs, zero modulus, inconsistent system, or
 *       lcm overflow past UINT32_MAX): return -1 (outs unchanged).
 *   int __gj_u32_crt_u  (alias)
 *   __libcgj_batch3315_marker = "libcgj-batch3315"
 *
 * CREATE-ONLY exclusive continuum wave (3311–3320). Unique gj_u32_crt_u
 * surface only; no multi-def. Distinct from gj_crt_pair (batch233). No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3315_marker[] = "libcgj-batch3315";

/* ---- freestanding helpers ---------------------------------------------- */

/* Euclidean GCD for non-negative uint32. */
static uint32_t
b3315_gcd(uint32_t u32A, uint32_t u32B)
{
	while (u32B != 0u) {
		uint32_t u32T = u32A % u32B;

		u32A = u32B;
		u32B = u32T;
	}
	return u32A;
}

/* Residue of a modulo m in [0, m). m must be > 0. */
static uint32_t
b3315_mod(uint32_t u32A, uint32_t u32M)
{
	return u32A % u32M;
}

/*
 * Modular inverse of a modulo m (m > 1), assuming gcd(a mod m, m) == 1.
 * Soft Bézout tracked in [0, m) — no signed overflow.
 * Returns inv in [0, m), or UINT32_MAX if no inverse.
 */
static uint32_t
b3315_modinv(uint32_t u32A, uint32_t u32M)
{
	uint32_t u32T = 0u;
	uint32_t u32NewT = 1u;
	uint32_t u32R = u32M;
	uint32_t u32NewR;

	u32A %= u32M;
	u32NewR = u32A;

	while (u32NewR != 0u) {
		uint32_t u32Q = u32R / u32NewR;
		uint32_t u32Tmp;
		uint32_t u32QN;
		uint32_t u32NextT;

		u32Tmp = u32R - u32Q * u32NewR;
		u32R = u32NewR;
		u32NewR = u32Tmp;

		/* next_t ≡ t - q*newt (mod m) */
		u32QN = (uint32_t)(((uint64_t)u32Q * (uint64_t)u32NewT) %
		                   (uint64_t)u32M);
		if (u32T >= u32QN) {
			u32NextT = u32T - u32QN;
		} else {
			u32NextT = u32M - (u32QN - u32T);
		}
		u32T = u32NewT;
		u32NewT = u32NextT;
	}

	if (u32R != 1u) {
		return UINT32_MAX;
	}
	return u32T % u32M;
}

/*
 * Multiply a * b into *pOut. Returns 0 on success, -1 if product would
 * exceed UINT32_MAX.
 */
static int
b3315_mul_ok(uint32_t u32A, uint32_t u32B, uint32_t *pOut)
{
	if (u32A == 0u || u32B == 0u) {
		*pOut = 0u;
		return 0;
	}
	if (u32A > (UINT32_MAX / u32B)) {
		return -1;
	}
	*pOut = u32A * u32B;
	return 0;
}

/* (a * b) mod m for a, b in [0, m), m > 1. Product fits in uint64. */
static uint32_t
b3315_mulmod(uint32_t u32A, uint32_t u32B, uint32_t u32M)
{
	return (uint32_t)(((uint64_t)u32A * (uint64_t)u32B) % (uint64_t)u32M);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_crt_u — solve the simultaneous system
 *   x ≡ a1 (mod m1)
 *   x ≡ a2 (mod m2)
 *
 * Existence: a1 ≡ a2 (mod gcd(m1, m2)).
 * Solution modulus: M = lcm(m1, m2) = (m1 / g) * m2.
 *
 * Construction (after normalizing residues into [0, mi)):
 *   g   = gcd(m1, m2)
 *   m1g = m1/g,  m2g = m2/g   (coprime)
 *   k   = ((a2 - a1)/g) * inv(m1g mod m2g)  mod m2g
 *   x   = a1 + m1 * k         already in [0, M) when a1 ∈ [0, m1)
 *
 * Returns 0 and writes *x, *m on success; -1 on error (no write).
 */
int
gj_u32_crt_u(uint32_t u32A1, uint32_t u32M1, uint32_t u32A2, uint32_t u32M2,
             uint32_t *pX, uint32_t *pM)
{
	uint32_t u32G;
	uint32_t u32M1g;
	uint32_t u32M2g;
	uint32_t u32R1;
	uint32_t u32R2;
	uint32_t u32Diff;
	uint32_t u32Inv;
	uint32_t u32K;
	uint32_t u32Lcm;
	uint32_t u32Prod;

	(void)NULL;

	if (pX == NULL || pM == NULL) {
		return -1;
	}
	if (u32M1 == 0u || u32M2 == 0u) {
		return -1;
	}

	u32R1 = b3315_mod(u32A1, u32M1);
	u32R2 = b3315_mod(u32A2, u32M2);

	u32G = b3315_gcd(u32M1, u32M2);
	/* Consistency: r1 ≡ r2 (mod g). */
	if (((u32R1 >= u32R2) ? (u32R1 - u32R2) : (u32R2 - u32R1)) % u32G != 0u) {
		return -1;
	}

	u32M1g = u32M1 / u32G;
	u32M2g = u32M2 / u32G;

	/* M = lcm = (m1 / g) * m2 */
	if (b3315_mul_ok(u32M1g, u32M2, &u32Lcm) != 0) {
		return -1;
	}

	/* Trivial when one modulus divides the other and residues agree. */
	if (u32M2g == 1u) {
		*pX = u32R1;
		*pM = u32Lcm;
		return 0;
	}

	/*
	 * k solves: m1g * k ≡ (r2 - r1)/g  (mod m2g)
	 * inv(m1g mod m2g) exists because gcd(m1g, m2g) == 1.
	 */
	u32Inv = b3315_modinv(u32M1g, u32M2g);
	if (u32Inv == UINT32_MAX) {
		return -1;
	}

	/* (r2 - r1) / g  lifted into [0, m2g). */
	if (u32R2 >= u32R1) {
		u32Diff = (u32R2 - u32R1) / u32G;
	} else {
		/* (r2 - r1) is negative; use (r2 + (g - (r1 % g)) - r1)/g wait —
		 * r1 ≡ r2 (mod g), so (r1 - r2) is divisible by g.
		 * (r2 - r1)/g ≡ -((r1 - r2)/g) (mod m2g). */
		u32Diff = u32M2g - (((u32R1 - u32R2) / u32G) % u32M2g);
		if (u32Diff == u32M2g) {
			u32Diff = 0u;
		}
	}
	u32Diff %= u32M2g;

	u32K = b3315_mulmod(u32Diff, u32Inv, u32M2g);

	/*
	 * x = r1 + m1 * k. Max = (m1-1) + m1*(m2g-1) = m1*m2g - 1 = M-1.
	 * m1 * k: k < m2g so m1*k <= m1*(m2g-1) < M <= UINT32_MAX.
	 */
	if (b3315_mul_ok(u32M1, u32K, &u32Prod) != 0) {
		return -1;
	}
	*pX = u32R1 + u32Prod;
	*pM = u32Lcm;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_crt_u(uint32_t u32A1, uint32_t u32M1, uint32_t u32A2, uint32_t u32M2,
                   uint32_t *pX, uint32_t *pM)
    __attribute__((alias("gj_u32_crt_u")));
