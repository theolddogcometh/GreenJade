/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3263: map uint64_t across two ranges (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_map_range_u(uint64_t x, uint64_t in_lo, uint64_t in_hi,
 *       uint64_t out_lo, uint64_t out_hi);
 *     - Linearly map x from the closed interval between in_lo and in_hi
 *       onto the closed interval from out_lo toward out_hi. x is clamped
 *       into the ordered source span first. Degenerate source span
 *       (in_lo == in_hi) yields out_lo. Destination direction is
 *       preserved when out_lo > out_hi. No __int128.
 *   uint64_t __gj_u64_map_range_u  (alias)
 *   __libcgj_batch3263_marker = "libcgj-batch3263"
 *
 * Milestone 3270 exclusive continuum CREATE-ONLY (3261-3270). Unique
 * gj_u64_map_range_u surface only; no multi-def. Distinct from
 * gj_u32_map_range_u (batch3262). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3263_marker[] = "libcgj-batch3263";

/* ---- freestanding helpers (no __int128) -------------------------------- */

/*
 * floor((hi:lo) / d) for d > 0. When hi >= d the mathematical quotient
 * is >= 2^64; saturate to UINT64_MAX.
 */
static uint64_t
b3263_div128_64(uint64_t u64Hi, uint64_t u64Lo, uint64_t u64D)
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
 * floor((a * b) / c) without __int128. Fast path when a*b fits in 64
 * bits; otherwise 32-bit limb product + 128/64 division.
 */
static uint64_t
b3263_mul_div(uint64_t u64A, uint64_t u64B, uint64_t u64C)
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

	if (u64A <= UINT64_MAX / u64B) {
		return (u64A * u64B) / u64C;
	}

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

	return b3263_div128_64(u64Hi, u64Lo, u64C);
}

/*
 * Linear map of x from [in_lo, in_hi] (direction-aware) onto
 * [out_lo → out_hi]. No parent wires; mul_div inlined above.
 */
static uint64_t
b3263_map_range(uint64_t u64X, uint64_t u64InLo, uint64_t u64InHi,
    uint64_t u64OutLo, uint64_t u64OutHi)
{
	uint64_t u64SrcLo;
	uint64_t u64SrcHi;
	uint64_t u64InSpan;
	uint64_t u64OutSpan;
	uint64_t u64Off;
	uint64_t u64Mix;

	if (u64InLo == u64InHi) {
		return u64OutLo;
	}

	if (u64InLo < u64InHi) {
		u64SrcLo = u64InLo;
		u64SrcHi = u64InHi;
	} else {
		u64SrcLo = u64InHi;
		u64SrcHi = u64InLo;
	}

	if (u64X < u64SrcLo) {
		u64X = u64SrcLo;
	} else if (u64X > u64SrcHi) {
		u64X = u64SrcHi;
	}

	if (u64InHi > u64InLo) {
		u64InSpan = u64InHi - u64InLo;
		u64Off = u64X - u64InLo;
	} else {
		u64InSpan = u64InLo - u64InHi;
		u64Off = u64InLo - u64X;
	}

	if (u64OutHi >= u64OutLo) {
		u64OutSpan = u64OutHi - u64OutLo;
		u64Mix = b3263_mul_div(u64Off, u64OutSpan, u64InSpan);
		return u64OutLo + u64Mix;
	}
	u64OutSpan = u64OutLo - u64OutHi;
	u64Mix = b3263_mul_div(u64Off, u64OutSpan, u64InSpan);
	return u64OutLo - u64Mix;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_map_range_u - map x from one uint64 range onto another.
 *
 * x:               value to map (clamped into ordered source span)
 * in_lo, in_hi:    source endpoints (may be inverted)
 * out_lo, out_hi:  destination endpoints (direction preserved)
 *
 * Degenerate source (in_lo == in_hi) returns out_lo. No __int128.
 * No parent wires.
 */
uint64_t
gj_u64_map_range_u(uint64_t u64X, uint64_t u64InLo, uint64_t u64InHi,
    uint64_t u64OutLo, uint64_t u64OutHi)
{
	(void)NULL;
	return b3263_map_range(u64X, u64InLo, u64InHi, u64OutLo, u64OutHi);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_map_range_u(uint64_t u64X, uint64_t u64InLo,
    uint64_t u64InHi, uint64_t u64OutLo, uint64_t u64OutHi)
    __attribute__((alias("gj_u64_map_range_u")));
