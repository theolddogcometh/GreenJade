/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3969: count trailing one bits in a uint64_t.
 *
 * Surface (unique symbols):
 *   unsigned gj_u64_trailing_ones_u(uint64_t x);
 *     - Number of consecutive 1-bits starting from bit 0 (LSB).
 *       Returns 64 when x is all-ones; returns 0 when LSB is clear.
 *       Equivalent to ctz(~x) with zero-input width convention.
 *   unsigned __gj_u64_trailing_ones_u  (alias)
 *   __libcgj_batch3969_marker = "libcgj-batch3969"
 *
 * Exclusive continuum CREATE-ONLY (3961-3970). Distinct from
 * gj_u64_ctz_u (batch2286), gj_u32_trailing_ones_u (batch3968), and
 * gj_u64_leading_ones_u (batch3967) — unique gj_u64_trailing_ones_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3969_marker[] = "libcgj-batch3969";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Binary-search peel of 32/16/8/4/2/1 against low masks on ~x.
 * All-ones input (~x == 0) -> 64.
 */
static unsigned
b3969_cto64(uint64_t u64X)
{
	unsigned uCto;
	uint64_t u64V;

	u64V = ~u64X;
	if (u64V == 0ull) {
		return 64u;
	}

	uCto = 0u;

	if ((u64V & 0xffffffffull) == 0ull) {
		uCto += 32u;
		u64V >>= 32;
	}
	if ((u64V & 0xffffull) == 0ull) {
		uCto += 16u;
		u64V >>= 16;
	}
	if ((u64V & 0xffull) == 0ull) {
		uCto += 8u;
		u64V >>= 8;
	}
	if ((u64V & 0xfull) == 0ull) {
		uCto += 4u;
		u64V >>= 4;
	}
	if ((u64V & 0x3ull) == 0ull) {
		uCto += 2u;
		u64V >>= 2;
	}
	if ((u64V & 0x1ull) == 0ull) {
		uCto += 1u;
	}

	return uCto;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_trailing_ones_u - count trailing ones in a 64-bit word.
 *
 * x: value to inspect
 *
 * Returns consecutive one count from LSB. x == 0 -> 0;
 * x == UINT64_MAX -> 64. Does not call libc.
 */
unsigned
gj_u64_trailing_ones_u(uint64_t u64X)
{
	(void)NULL;
	return b3969_cto64(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u64_trailing_ones_u(uint64_t u64X)
    __attribute__((alias("gj_u64_trailing_ones_u")));
