/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2506: modular inverse for odd modulus (uint64).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_invmod_odd(uint64_t a, uint64_t m);
 *     — multiplicative inverse of a modulo odd m via soft extended
 *       Euclidean (coefficients reduced mod m; no signed overflow).
 *       Returns inv in [0, m) with (a * inv) % m == 1 when gcd(a, m)==1.
 *       Returns 0 if m is even / m <= 1 / no inverse exists / a ≡ 0.
 *   uint64_t __gj_u64_invmod_odd  (alias)
 *   __libcgj_batch2506_marker = "libcgj-batch2506"
 *
 * Post-2500 number-theory exclusive wave (2501–2510). Unique
 * gj_u64_invmod_odd surface; no multi-def. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2506_marker[] = "libcgj-batch2506";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * (a * b) mod m for m > 0. Binary mulmod; freestanding-safe.
 */
static uint64_t
b2506_mulmod(uint64_t u64A, uint64_t u64B, uint64_t u64M)
{
	uint64_t u64R;

	u64A %= u64M;
	u64B %= u64M;
	u64R = 0ull;
	while (u64B != 0ull) {
		if ((u64B & 1ull) != 0ull) {
			uint64_t u64S = u64M - u64R;

			u64R = (u64A >= u64S) ? (u64A - u64S) : (u64R + u64A);
		}
		{
			uint64_t u64S = u64M - u64A;

			u64A = (u64A >= u64S) ? (u64A - u64S) : (u64A + u64A);
		}
		u64B >>= 1;
	}
	return u64R;
}

/*
 * Soft extended Euclid modular inverse for odd m > 1.
 * Tracks Bézout coefficient of a reduced into [0, m) at every step
 * so no signed overflow can occur on full uint64 range.
 *
 * Invariant: a * t ≡ r (mod m) for the current (r, t) pair after each
 * remainder update (starting with a * 1 ≡ a, a * 0 ≡ 0 vs m).
 * When newr hits 0, r holds gcd and t holds a^{-1} if gcd == 1.
 */
static uint64_t
b2506_invmod_odd(uint64_t u64A, uint64_t u64M)
{
	uint64_t u64T;
	uint64_t u64NewT;
	uint64_t u64R;
	uint64_t u64NewR;

	if (u64M <= 1ull || (u64M & 1ull) == 0ull) {
		return 0ull;
	}

	u64A %= u64M;
	if (u64A == 0ull) {
		return 0ull;
	}

	u64T = 0ull;
	u64NewT = 1ull;
	u64R = u64M;
	u64NewR = u64A;

	while (u64NewR != 0ull) {
		uint64_t u64Q = u64R / u64NewR;
		uint64_t u64Tmp;
		uint64_t u64QN;
		uint64_t u64NextT;

		/* r, newr = newr, r - q * newr  (exact, product fits). */
		u64Tmp = u64R - u64Q * u64NewR;
		u64R = u64NewR;
		u64NewR = u64Tmp;

		/*
		 * t, newt = newt, t - q * newt  (mod m), soft non-neg:
		 * next_t ≡ t - (q * newt) (mod m).
		 */
		u64QN = b2506_mulmod(u64Q, u64NewT, u64M);
		if (u64T >= u64QN) {
			u64NextT = u64T - u64QN;
		} else {
			u64NextT = u64M - (u64QN - u64T);
		}
		u64T = u64NewT;
		u64NewT = u64NextT;
	}

	/* gcd is u64R; need exactly 1 for a unit. */
	if (u64R != 1ull) {
		return 0ull;
	}
	return u64T % u64M;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_invmod_odd — modular inverse of a modulo odd m.
 *
 * a: value to invert (reduced mod m internally)
 * m: odd modulus (> 1 required; even / m<=1 → 0)
 *
 * Returns inv in [0, m) with a*inv ≡ 1 (mod m), or 0 if none.
 */
uint64_t
gj_u64_invmod_odd(uint64_t u64A, uint64_t u64M)
{
	(void)NULL;
	return b2506_invmod_odd(u64A, u64M);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_invmod_odd(uint64_t u64A, uint64_t u64M)
    __attribute__((alias("gj_u64_invmod_odd")));
