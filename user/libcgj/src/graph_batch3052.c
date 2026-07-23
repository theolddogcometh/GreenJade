/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3052: portable uint64 multiply-then-divide
 * without __int128.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_mul_div_u(uint64_t a, uint64_t b, uint64_t c);
 *     - Return floor((a * b) / c). Uses a 32-bit limb full product and
 *       128/64 long division when a*b would overflow uint64_t. c==0 → 0.
 *       If the mathematical quotient exceeds UINT64_MAX, saturates to
 *       UINT64_MAX.
 *   uint64_t __gj_u64_mul_div_u  (alias)
 *   __libcgj_batch3052_marker = "libcgj-batch3052"
 *
 * CREATE-ONLY exclusive continuum wave (3051-3060). Unique
 * gj_u64_mul_div_u surface only; no parent wires, no multi-def.
 * Portable: no __int128 / no libgcc soft-quad helpers.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3052_marker[] = "libcgj-batch3052";

/* ---- freestanding helpers (no __int128) -------------------------------- */

/*
 * floor((hi:lo) / d) for d > 0. When hi >= d the mathematical quotient
 * is >= 2^64; saturate to UINT64_MAX. Otherwise a 64-iteration binary
 * long division yields an exact 64-bit quotient.
 */
static uint64_t
b3052_div128_64(uint64_t u64Hi, uint64_t u64Lo, uint64_t u64D)
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
 * floor((a * b) / c). Fast path when a*b fits in 64 bits; otherwise
 * form the full product with 32-bit limbs and divide the 128-bit
 * intermediate by c. No __int128.
 */
static uint64_t
b3052_mul_div(uint64_t u64A, uint64_t u64B, uint64_t u64C)
{
	uint64_t u64Hi;
	uint64_t u64Lo;
	uint64_t u64Al, u64Ah, u64Bl, u64Bh;
	uint64_t u64P00, u64P01, u64P10, u64P11;
	uint64_t u64Mid;

	if (u64C == 0ull) {
		return 0ull;
	}
	if (u64A == 0ull || u64B == 0ull) {
		return 0ull;
	}

	/* Fast path: product fits in 64 bits. */
	if (u64A <= UINT64_MAX / u64B) {
		return (u64A * u64B) / u64C;
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

	return b3052_div128_64(u64Hi, u64Lo, u64C);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_mul_div_u - portable floor((a * b) / c) without __int128.
 *
 * a, b: factors
 * c:    divisor (c==0 yields 0)
 *
 * When a*b fits in 64 bits, uses a direct product/divide. Otherwise
 * forms a 128-bit product from 32-bit limbs and divides by c. Quotients
 * that would exceed UINT64_MAX saturate to UINT64_MAX.
 */
uint64_t
gj_u64_mul_div_u(uint64_t u64A, uint64_t u64B, uint64_t u64C)
{
	(void)NULL;
	return b3052_mul_div(u64A, u64B, u64C);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_mul_div_u(uint64_t u64A, uint64_t u64B, uint64_t u64C)
    __attribute__((alias("gj_u64_mul_div_u")));
