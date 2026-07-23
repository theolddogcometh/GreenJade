/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4056: uint64_t lerp with unorm weight
 * (exclusive lerp_unorm_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_lerp_unorm_u(uint64_t a, uint64_t b, uint64_t t);
 *     - Linear interpolate a→b with t as unorm64 in [0, UINT64_MAX]
 *       mapped to [0.0, 1.0]. t==0 → a; t==UINT64_MAX → b.
 *       Formula: a ± floor(|b-a| * t / UINT64_MAX).
 *   uint64_t __gj_u64_lerp_unorm_u  (alias)
 *   __libcgj_batch4056_marker = "libcgj-batch4056"
 *
 * CREATE-ONLY exclusive continuum wave (4051-4060). Unique
 * gj_u64_lerp_unorm_u surface only; no multi-def. Distinct from
 * gj_u64_lerp_q16_u (batch2649), gj_u64_lerp_q16_u2 (batch3458),
 * gj_u32_lerp_unorm_u (batch4055). No parent wires. No __int128: wide
 * product via 32-bit limbs + 128/64 long division.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4056_marker[] = "libcgj-batch4056";

/* ---- freestanding helpers (no __int128) -------------------------------- */

/*
 * floor((hi:lo) / d) for d > 0. When hi >= d the mathematical quotient
 * is >= 2^64; saturate to UINT64_MAX.
 */
static uint64_t
b4056_div128_64(uint64_t u64Hi, uint64_t u64Lo, uint64_t u64D)
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

/* Floor of (x * t) / UINT64_MAX without __int128. */
static uint64_t
b4056_mul_div_max(uint64_t u64X, uint64_t u64T)
{
	uint64_t u64Hi;
	uint64_t u64Lo;
	uint64_t u64Al, u64Ah, u64Bl, u64Bh;
	uint64_t u64P00, u64P01, u64P10, u64P11;
	uint64_t u64Mid;

	if (u64X == 0ull || u64T == 0ull) {
		return 0ull;
	}
	if (u64T == UINT64_MAX) {
		return u64X;
	}
	if (u64X == UINT64_MAX) {
		return u64T;
	}

	/* Fast path: product fits in 64 bits. */
	if (u64X <= UINT64_MAX / u64T) {
		return (u64X * u64T) / UINT64_MAX;
	}

	u64Al = u64X & 0xffffffffull;
	u64Ah = u64X >> 32;
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

	return b4056_div128_64(u64Hi, u64Lo, UINT64_MAX);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_lerp_unorm_u — unsigned lerp of a,b with unorm64 weight t.
 *
 * t==0 returns a; t==UINT64_MAX returns b. Direction uses abs-diff so
 * a>b works without unsigned wrap of (b - a). Mix scaled without
 * __int128. Does not call libc. No parent wires.
 */
uint64_t
gj_u64_lerp_unorm_u(uint64_t u64A, uint64_t u64B, uint64_t u64T)
{
	uint64_t u64Diff;
	uint64_t u64Mix;

	(void)NULL;
	if (u64T == 0ull) {
		return u64A;
	}
	if (u64T == UINT64_MAX) {
		return u64B;
	}
	if (u64A == u64B) {
		return u64A;
	}
	if (u64B >= u64A) {
		u64Diff = u64B - u64A;
		u64Mix = b4056_mul_div_max(u64Diff, u64T);
		return u64A + u64Mix;
	}
	u64Diff = u64A - u64B;
	u64Mix = b4056_mul_div_max(u64Diff, u64T);
	return u64A - u64Mix;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_lerp_unorm_u(uint64_t u64A, uint64_t u64B, uint64_t u64T)
    __attribute__((alias("gj_u64_lerp_unorm_u")));
