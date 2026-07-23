/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2944: modular inverse on uint32_t (_u surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_inv_mod_u32_u(uint32_t a, uint32_t m);
 *     — multiplicative inverse of a modulo m via soft extended Euclidean
 *       (coefficients reduced mod m; no signed overflow).
 *       Returns inv in [0, m) with (a * inv) % m == 1 when gcd(a, m)==1.
 *       Returns 0 if m <= 1 / no inverse exists / a ≡ 0 (mod m).
 *   uint32_t __gj_inv_mod_u32_u  (alias)
 *   __libcgj_batch2944_marker = "libcgj-batch2944"
 *
 * CREATE-ONLY exclusive continuum wave (2941–2950). Distinct from
 * gj_u64_invmod_odd (batch2506) — unique gj_inv_mod_u32_u name; no
 * parent wires, no multi-def. Works for any m > 1 with gcd(a,m)==1.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2944_marker[] = "libcgj-batch2944";

/* ---- freestanding helpers ---------------------------------------------- */

/* (a * b) mod m for m > 0. Local binary mulmod; freestanding-safe. */
static uint32_t
b2944_mulmod(uint32_t u32A, uint32_t u32B, uint32_t u32M)
{
	uint32_t u32R;

	u32A %= u32M;
	u32B %= u32M;
	u32R = 0u;
	while (u32B != 0u) {
		if ((u32B & 1u) != 0u) {
			uint32_t u32S = u32M - u32R;

			u32R = (u32A >= u32S) ? (u32A - u32S) : (u32R + u32A);
		}
		{
			uint32_t u32S = u32M - u32A;

			u32A = (u32A >= u32S) ? (u32A - u32S) : (u32A + u32A);
		}
		u32B >>= 1;
	}
	return u32R;
}

/*
 * Soft extended Euclid modular inverse for m > 1.
 * Tracks Bézout coefficient of a reduced into [0, m) at every step
 * so no signed overflow can occur on full uint32 range.
 *
 * Invariant: a * t ≡ r (mod m) for the current (r, t) pair after each
 * remainder update. When newr hits 0, r holds gcd and t holds a^{-1}
 * if gcd == 1.
 */
static uint32_t
b2944_invmod(uint32_t u32A, uint32_t u32M)
{
	uint32_t u32T;
	uint32_t u32NewT;
	uint32_t u32R;
	uint32_t u32NewR;

	if (u32M <= 1u) {
		return 0u;
	}

	u32A %= u32M;
	if (u32A == 0u) {
		return 0u;
	}

	u32T = 0u;
	u32NewT = 1u;
	u32R = u32M;
	u32NewR = u32A;

	while (u32NewR != 0u) {
		uint32_t u32Q = u32R / u32NewR;
		uint32_t u32Tmp;
		uint32_t u32QN;
		uint32_t u32NextT;

		/* r, newr = newr, r - q * newr  (exact, product fits). */
		u32Tmp = u32R - u32Q * u32NewR;
		u32R = u32NewR;
		u32NewR = u32Tmp;

		/*
		 * t, newt = newt, t - q * newt  (mod m), soft non-neg:
		 * next_t ≡ t - (q * newt) (mod m).
		 */
		u32QN = b2944_mulmod(u32Q, u32NewT, u32M);
		if (u32T >= u32QN) {
			u32NextT = u32T - u32QN;
		} else {
			u32NextT = u32M - (u32QN - u32T);
		}
		u32T = u32NewT;
		u32NewT = u32NextT;
	}

	/* gcd is u32R; need exactly 1 for a unit. */
	if (u32R != 1u) {
		return 0u;
	}
	return u32T % u32M;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_inv_mod_u32_u — modular inverse of a modulo m.
 *
 * a: value to invert (reduced mod m internally)
 * m: modulus (> 1 required; m<=1 → 0)
 *
 * Returns inv in [0, m) with a*inv ≡ 1 (mod m), or 0 if none.
 */
uint32_t
gj_inv_mod_u32_u(uint32_t u32A, uint32_t u32M)
{
	(void)NULL;
	return b2944_invmod(u32A, u32M);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_inv_mod_u32_u(uint32_t u32A, uint32_t u32M)
    __attribute__((alias("gj_inv_mod_u32_u")));
