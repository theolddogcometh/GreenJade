/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4058: uint64_t inverse lerp to unorm
 * (exclusive inv_lerp_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_inv_lerp_u(uint64_t a, uint64_t b, uint64_t v);
 *     - Inverse of unorm lerp: unorm t such that
 *       lerp_unorm(a, b, t) ≈ v. Returns floor(|v-a| * UINT64_MAX /
 *       |b-a|), clamped to [0, UINT64_MAX]. a==b → 0.
 *   uint64_t __gj_u64_inv_lerp_u  (alias)
 *   __libcgj_batch4058_marker = "libcgj-batch4058"
 *
 * CREATE-ONLY exclusive continuum wave (4051-4060). Unique
 * gj_u64_inv_lerp_u surface only; no multi-def. Companion of
 * gj_u64_lerp_unorm_u (batch4056) but self-contained (no parent wires).
 * No __int128: wide product via 32-bit limbs + 128/64 long division.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4058_marker[] = "libcgj-batch4058";

/* ---- freestanding helpers (no __int128) -------------------------------- */

static uint64_t
b4058_div128_64(uint64_t u64Hi, uint64_t u64Lo, uint64_t u64D)
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

/* Floor of (num * UINT64_MAX) / den for den > 0. No __int128. */
static uint64_t
b4058_scale_to_unorm(uint64_t u64Num, uint64_t u64Den)
{
	uint64_t u64Hi;
	uint64_t u64Lo;
	uint64_t u64Al, u64Ah, u64Bl, u64Bh;
	uint64_t u64P00, u64P01, u64P10, u64P11;
	uint64_t u64Mid;

	if (u64Num == 0ull) {
		return 0ull;
	}
	if (u64Num >= u64Den) {
		return UINT64_MAX;
	}

	/* Fast path: num * UINT64_MAX would need full width always when
	 * num > 1; use limbs whenever num > 1, but try overflow-safe. */
	if (u64Num == 1ull) {
		return UINT64_MAX / u64Den;
	}

	/* Product num * UINT64_MAX = (num << 64) - num = (num-1):(~num+1)
	 * Wait: num * (2^64 - 1) = num*2^64 - num → hi=num, lo=-num
	 * (two's complement). Use limb multiply instead for clarity. */
	u64Al = u64Num & 0xffffffffull;
	u64Ah = u64Num >> 32;
	u64Bl = UINT64_MAX & 0xffffffffull; /* 0xffffffff */
	u64Bh = UINT64_MAX >> 32;           /* 0xffffffff */

	u64P00 = u64Al * u64Bl;
	u64P01 = u64Al * u64Bh;
	u64P10 = u64Ah * u64Bl;
	u64P11 = u64Ah * u64Bh;

	u64Mid = (u64P00 >> 32) + (u64P01 & 0xffffffffull) +
	    (u64P10 & 0xffffffffull);
	u64Lo = (u64P00 & 0xffffffffull) | (u64Mid << 32);
	u64Hi = u64P11 + (u64P01 >> 32) + (u64P10 >> 32) + (u64Mid >> 32);

	return b4058_div128_64(u64Hi, u64Lo, u64Den);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_inv_lerp_u — unorm weight of v between a and b.
 *
 * a, b: range endpoints (order may be inverted)
 * v:    sample value
 *
 * When a < b:
 *   v <= a → 0; v >= b → UINT64_MAX; else floor((v-a)*MAX/(b-a)).
 * When a > b:
 *   v >= a → 0; v <= b → UINT64_MAX; else floor((a-v)*MAX/(a-b)).
 * When a == b: returns 0. Self-contained; no parent wires.
 */
uint64_t
gj_u64_inv_lerp_u(uint64_t u64A, uint64_t u64B, uint64_t u64V)
{
	uint64_t u64Span;
	uint64_t u64Off;

	(void)NULL;
	if (u64A == u64B) {
		return 0ull;
	}
	if (u64B > u64A) {
		if (u64V <= u64A) {
			return 0ull;
		}
		if (u64V >= u64B) {
			return UINT64_MAX;
		}
		u64Span = u64B - u64A;
		u64Off = u64V - u64A;
		return b4058_scale_to_unorm(u64Off, u64Span);
	}
	if (u64V >= u64A) {
		return 0ull;
	}
	if (u64V <= u64B) {
		return UINT64_MAX;
	}
	u64Span = u64A - u64B;
	u64Off = u64A - u64V;
	return b4058_scale_to_unorm(u64Off, u64Span);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_inv_lerp_u(uint64_t u64A, uint64_t u64B, uint64_t u64V)
    __attribute__((alias("gj_u64_inv_lerp_u")));
