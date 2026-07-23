/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4483: uint64 multiply-then-divide without
 * __int128 (GCD reduce + long division).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_mul_div_u2(uint64_t a, uint64_t b, uint64_t d);
 *     - Return floor((a * b) / d). Reduces by GCD first, then uses a
 *       32-bit-limb full product and 128/64 long division when needed.
 *       d==0 → 0. Quotients exceeding UINT64_MAX saturate to
 *       UINT64_MAX.
 *   uint64_t __gj_u64_mul_div_u2  (alias)
 *   __libcgj_batch4483_marker = "libcgj-batch4483"
 *
 * Exclusive continuum CREATE-ONLY (4481-4490: mul_div_u2, scale_u,
 * u64_mul_div_u2, percent_u, basis_points_u, lerp_u, inv_lerp_u2,
 * smoothstep_tmax_u, clamp01_frac_u, batch_id_4490). Float-free
 * rational unique wave. Distinct from gj_u64_mul_div_u (batch3052) —
 * unique _u2 surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4483_marker[] = "libcgj-batch4483";

/* ---- freestanding helpers (no __int128) -------------------------------- */

/* Binary GCD (Stein's algorithm). */
static uint64_t
b4483_gcd(uint64_t u64X, uint64_t u64Y)
{
	unsigned uShift;

	if (u64X == 0ull) {
		return u64Y;
	}
	if (u64Y == 0ull) {
		return u64X;
	}

	uShift = 0u;
	while (((u64X | u64Y) & 1ull) == 0ull) {
		u64X >>= 1;
		u64Y >>= 1;
		uShift++;
	}
	while ((u64X & 1ull) == 0ull) {
		u64X >>= 1;
	}
	do {
		while ((u64Y & 1ull) == 0ull) {
			u64Y >>= 1;
		}
		if (u64X > u64Y) {
			uint64_t u64T = u64X;
			u64X = u64Y;
			u64Y = u64T;
		}
		u64Y -= u64X;
	} while (u64Y != 0ull);

	return u64X << uShift;
}

/*
 * floor((hi:lo) / d) for d > 0. When hi >= d the mathematical quotient
 * is >= 2^64; saturate to UINT64_MAX. Otherwise a 64-iteration binary
 * long division yields an exact 64-bit quotient.
 */
static uint64_t
b4483_div128_64(uint64_t u64Hi, uint64_t u64Lo, uint64_t u64D)
{
	uint64_t u64Q;
	uint64_t u64R;
	uint32_t uBit;

	if (u64Hi >= u64D) {
		return UINT64_MAX;
	}

	u64Q = 0ull;
	u64R = u64Hi;
	for (uBit = 0u; uBit < 64u; uBit++) {
		uint64_t u64Msb = u64R >> 63;
		uint64_t u64NewR = (u64R << 1) | (u64Lo >> 63);

		u64Lo <<= 1;
		u64Q <<= 1;
		u64R = u64NewR;
		if (u64Msb != 0ull || u64R >= u64D) {
			u64R -= u64D;
			u64Q |= 1ull;
		}
	}
	return u64Q;
}

/*
 * floor((a * b) / d). Reduce by GCD, then fast path when a*b fits in
 * 64 bits; otherwise form the full product with 32-bit limbs and
 * divide the 128-bit intermediate by d. No __int128.
 */
static uint64_t
b4483_mul_div(uint64_t u64A, uint64_t u64B, uint64_t u64D)
{
	uint64_t u64G;
	uint64_t u64Hi;
	uint64_t u64Lo;
	uint64_t u64Al, u64Ah, u64Bl, u64Bh;
	uint64_t u64P00, u64P01, u64P10, u64P11;
	uint64_t u64Mid;

	if (u64D == 0ull) {
		return 0ull;
	}
	if (u64A == 0ull || u64B == 0ull) {
		return 0ull;
	}

	/* Cancel common factors: gcd(a,d) then gcd(b,d). */
	u64G = b4483_gcd(u64A, u64D);
	if (u64G > 1ull) {
		u64A /= u64G;
		u64D /= u64G;
	}
	u64G = b4483_gcd(u64B, u64D);
	if (u64G > 1ull) {
		u64B /= u64G;
		u64D /= u64G;
	}

	/* Fast path: product fits in 64 bits. */
	if (u64A <= UINT64_MAX / u64B) {
		return (u64A * u64B) / u64D;
	}

	/* Full product via 32-bit limbs (no __int128). */
	u64Al = u64A & 0xffffffffull;
	u64Ah = u64A >> 32;
	u64Bl = u64B & 0xffffffffull;
	u64Bh = u64B >> 32;

	u64P00 = u64Al * u64Bl;
	u64P01 = u64Al * u64Bh;
	u64P10 = u64Ah * u64Bl;
	u64P11 = u64Ah * u64Bh;

	u64Mid = (u64P00 >> 32) + (u64P01 & 0xffffffffull) +
	    (u64P10 & 0xffffffffull);
	u64Lo = (u64P00 & 0xffffffffull) | (u64Mid << 32);
	u64Hi = u64P11 + (u64P01 >> 32) + (u64P10 >> 32) + (u64Mid >> 32);

	return b4483_div128_64(u64Hi, u64Lo, u64D);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_mul_div_u2 - portable floor((a * b) / d) without __int128.
 *
 * a, b: factors
 * d:    divisor (d==0 yields 0)
 *
 * Reduces a,b,d by GCD first, then multiplies / divides. When a*b fits
 * in 64 bits, uses a direct product/divide. Otherwise forms a 128-bit
 * product from 32-bit limbs and divides by d. Quotients that would
 * exceed UINT64_MAX saturate to UINT64_MAX.
 */
uint64_t
gj_u64_mul_div_u2(uint64_t u64A, uint64_t u64B, uint64_t u64D)
{
	(void)NULL;
	return b4483_mul_div(u64A, u64B, u64D);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_mul_div_u2(uint64_t u64A, uint64_t u64B, uint64_t u64D)
    __attribute__((alias("gj_u64_mul_div_u2")));
