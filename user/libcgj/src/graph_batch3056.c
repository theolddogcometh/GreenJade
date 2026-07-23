/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3056: unsigned uint64 linear interpolation.
 *
 * Surface (unique symbols):
 *   uint64_t gj_lerp_u64_u(uint64_t a, uint64_t b, uint64_t t,
 *                          uint64_t t_max);
 *     - Return a + (b - a) * t / t_max. Portable path without __int128:
 *       when (diff * t) would overflow, uses a limb product + 128/64
 *       divide. t_max == 0 → a. Handles b < a without underflow.
 *   uint64_t __gj_lerp_u64_u  (alias)
 *   __libcgj_batch3056_marker = "libcgj-batch3056"
 *
 * CREATE-ONLY exclusive continuum wave (3051-3060). Unique
 * gj_lerp_u64_u surface only; no parent wires, no multi-def.
 * Distinct from gj_lerp_i32 (batch959) and gj_lerp_u32_u (batch3055).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3056_marker[] = "libcgj-batch3056";

/* ---- freestanding helpers (no __int128) -------------------------------- */

/*
 * floor((diff * t) / t_max) for unsigned operands. Fast path when the
 * product fits; otherwise 32-bit limb product + binary long division.
 */
static uint64_t
b3056_mul_div(uint64_t u64Diff, uint64_t u64T, uint64_t u64TMax)
{
	uint64_t u64Hi, u64Lo;
	uint64_t u64Al, u64Ah, u64Bl, u64Bh;
	uint64_t u64P00, u64P01, u64P10, u64P11;
	uint64_t u64Mid;
	uint64_t u64Q, u64R;
	uint32_t uBit;

	if (u64TMax == 0ull) {
		return 0ull;
	}
	if (u64Diff == 0ull || u64T == 0ull) {
		return 0ull;
	}
	if (u64Diff <= UINT64_MAX / u64T) {
		return (u64Diff * u64T) / u64TMax;
	}

	u64Al = u64Diff & 0xffffffffull;
	u64Ah = u64Diff >> 32;
	u64Bl = u64T & 0xffffffffull;
	u64Bh = u64T >> 32;

	u64P00 = u64Al * u64Bl;
	u64P01 = u64Al * u64Bh;
	u64P10 = u64Ah * u64Bl;
	u64P11 = u64Ah * u64Bh;

	u64Mid = (u64P00 >> 32) + (u64P01 & 0xffffffffull) +
	    (u64P10 & 0xffffffffull);
	u64Lo = (u64P00 & 0xffffffffull) | (u64Mid << 32);
	u64Hi = u64P11 + (u64P01 >> 32) + (u64P10 >> 32) + (u64Mid >> 32);

	if (u64Hi >= u64TMax) {
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
		if (u64Msb != 0ull || u64R >= u64TMax) {
			u64R -= u64TMax;
			u64Q |= 1ull;
		}
	}
	return u64Q;
}

static uint64_t
b3056_lerp(uint64_t u64A, uint64_t u64B, uint64_t u64T, uint64_t u64TMax)
{
	uint64_t u64Step;

	if (u64TMax == 0ull) {
		return u64A;
	}
	if (u64B >= u64A) {
		u64Step = b3056_mul_div(u64B - u64A, u64T, u64TMax);
		return u64A + u64Step;
	}
	u64Step = b3056_mul_div(u64A - u64B, u64T, u64TMax);
	return u64A - u64Step;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lerp_u64_u - unsigned linear interpolation on uint64_t.
 *
 * a, b: endpoints
 * t:    blend parameter (caller-owned range; typically 0..t_max)
 * t_max: blend scale; t_max==0 returns a
 *
 * Portable: no __int128. Self-contained; no parent wires.
 */
uint64_t
gj_lerp_u64_u(uint64_t u64A, uint64_t u64B, uint64_t u64T, uint64_t u64TMax)
{
	(void)NULL;
	return b3056_lerp(u64A, u64B, u64T, u64TMax);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_lerp_u64_u(uint64_t u64A, uint64_t u64B, uint64_t u64T,
    uint64_t u64TMax)
    __attribute__((alias("gj_lerp_u64_u")));
