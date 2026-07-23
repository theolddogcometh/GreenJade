/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3966: count leading one bits in a uint32_t.
 *
 * Surface (unique symbols):
 *   unsigned gj_u32_leading_ones_u(uint32_t x);
 *     - Number of consecutive 1-bits starting from bit 31 (MSB).
 *       Returns 32 when x is all-ones; returns 0 when MSB is clear.
 *       Equivalent to clz(~x) with zero-input width convention.
 *   unsigned __gj_u32_leading_ones_u  (alias)
 *   __libcgj_batch3966_marker = "libcgj-batch3966"
 *
 * Exclusive continuum CREATE-ONLY (3961-3970). Distinct from
 * gj_u32_clz_u (batch2287), gj_clz_u32 (batch788), and
 * gj_u32_trailing_ones_u (batch3968) — unique gj_u32_leading_ones_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3966_marker[] = "libcgj-batch3966";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Binary-search peel of 16/8/4/2/1 against high half-masks on ~x.
 * All-ones input (~x == 0) -> 32.
 */
static unsigned
b3966_clo32(uint32_t u32X)
{
	unsigned uClo;
	uint32_t u32V;

	u32V = ~u32X;
	if (u32V == 0u) {
		return 32u;
	}

	uClo = 0u;

	if ((u32V & 0xffff0000u) == 0u) {
		uClo += 16u;
		u32V <<= 16;
	}
	if ((u32V & 0xff000000u) == 0u) {
		uClo += 8u;
		u32V <<= 8;
	}
	if ((u32V & 0xf0000000u) == 0u) {
		uClo += 4u;
		u32V <<= 4;
	}
	if ((u32V & 0xc0000000u) == 0u) {
		uClo += 2u;
		u32V <<= 2;
	}
	if ((u32V & 0x80000000u) == 0u) {
		uClo += 1u;
	}

	return uClo;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_leading_ones_u - count leading ones in a 32-bit word.
 *
 * x: value to inspect
 *
 * Returns consecutive one count from MSB. x == 0 -> 0;
 * x == 0xffffffff -> 32. Does not call libc.
 */
unsigned
gj_u32_leading_ones_u(uint32_t u32X)
{
	(void)NULL;
	return b3966_clo32(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u32_leading_ones_u(uint32_t u32X)
    __attribute__((alias("gj_u32_leading_ones_u")));
