/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3967: count leading one bits in a uint64_t.
 *
 * Surface (unique symbols):
 *   unsigned gj_u64_leading_ones_u(uint64_t x);
 *     - Number of consecutive 1-bits starting from bit 63 (MSB).
 *       Returns 64 when x is all-ones; returns 0 when MSB is clear.
 *       Equivalent to clz(~x) with zero-input width convention.
 *   unsigned __gj_u64_leading_ones_u  (alias)
 *   __libcgj_batch3967_marker = "libcgj-batch3967"
 *
 * Exclusive continuum CREATE-ONLY (3961-3970). Distinct from
 * gj_u64_clz_u (batch2288), gj_u32_leading_ones_u (batch3966), and
 * gj_clz64 (batch367) — unique gj_u64_leading_ones_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3967_marker[] = "libcgj-batch3967";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Binary-search peel of 32/16/8/4/2/1 against high half-masks on ~x.
 * All-ones input (~x == 0) -> 64.
 */
static unsigned
b3967_clo64(uint64_t u64X)
{
	unsigned uClo;
	uint64_t u64V;

	u64V = ~u64X;
	if (u64V == 0ull) {
		return 64u;
	}

	uClo = 0u;

	if ((u64V & 0xffffffff00000000ull) == 0ull) {
		uClo += 32u;
		u64V <<= 32;
	}
	if ((u64V & 0xffff000000000000ull) == 0ull) {
		uClo += 16u;
		u64V <<= 16;
	}
	if ((u64V & 0xff00000000000000ull) == 0ull) {
		uClo += 8u;
		u64V <<= 8;
	}
	if ((u64V & 0xf000000000000000ull) == 0ull) {
		uClo += 4u;
		u64V <<= 4;
	}
	if ((u64V & 0xc000000000000000ull) == 0ull) {
		uClo += 2u;
		u64V <<= 2;
	}
	if ((u64V & 0x8000000000000000ull) == 0ull) {
		uClo += 1u;
	}

	return uClo;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_leading_ones_u - count leading ones in a 64-bit word.
 *
 * x: value to inspect
 *
 * Returns consecutive one count from MSB. x == 0 -> 0;
 * x == UINT64_MAX -> 64. Does not call libc.
 */
unsigned
gj_u64_leading_ones_u(uint64_t u64X)
{
	(void)NULL;
	return b3967_clo64(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u64_leading_ones_u(uint64_t u64X)
    __attribute__((alias("gj_u64_leading_ones_u")));
