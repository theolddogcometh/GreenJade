/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3968: count trailing one bits in a uint32_t.
 *
 * Surface (unique symbols):
 *   unsigned gj_u32_trailing_ones_u(uint32_t x);
 *     - Number of consecutive 1-bits starting from bit 0 (LSB).
 *       Returns 32 when x is all-ones; returns 0 when LSB is clear.
 *       Equivalent to ctz(~x) with zero-input width convention.
 *   unsigned __gj_u32_trailing_ones_u  (alias)
 *   __libcgj_batch3968_marker = "libcgj-batch3968"
 *
 * Exclusive continuum CREATE-ONLY (3961-3970). Distinct from
 * gj_u32_ctz_u (batch2285), gj_u32_leading_ones_u (batch3966), and
 * gj_ctz_u32 (batch787) — unique gj_u32_trailing_ones_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3968_marker[] = "libcgj-batch3968";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Binary-search peel of 16/8/4/2/1 against low masks on ~x.
 * All-ones input (~x == 0) -> 32.
 */
static unsigned
b3968_cto32(uint32_t u32X)
{
	unsigned uCto;
	uint32_t u32V;

	u32V = ~u32X;
	if (u32V == 0u) {
		return 32u;
	}

	uCto = 0u;

	if ((u32V & 0xffffu) == 0u) {
		uCto += 16u;
		u32V >>= 16;
	}
	if ((u32V & 0xffu) == 0u) {
		uCto += 8u;
		u32V >>= 8;
	}
	if ((u32V & 0xfu) == 0u) {
		uCto += 4u;
		u32V >>= 4;
	}
	if ((u32V & 0x3u) == 0u) {
		uCto += 2u;
		u32V >>= 2;
	}
	if ((u32V & 0x1u) == 0u) {
		uCto += 1u;
	}

	return uCto;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_trailing_ones_u - count trailing ones in a 32-bit word.
 *
 * x: value to inspect
 *
 * Returns consecutive one count from LSB. x == 0 -> 0;
 * x == 0xffffffff -> 32. Does not call libc.
 */
unsigned
gj_u32_trailing_ones_u(uint32_t u32X)
{
	(void)NULL;
	return b3968_cto32(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u32_trailing_ones_u(uint32_t u32X)
    __attribute__((alias("gj_u32_trailing_ones_u")));
