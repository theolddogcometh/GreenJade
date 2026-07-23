/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2287: count leading zero bits in a uint32_t.
 *
 * Surface (unique symbols):
 *   unsigned gj_u32_clz_u(uint32_t x);
 *     - Number of consecutive 0-bits starting from bit 31 (MSB).
 *       Returns 32 when x == 0 (fully defined for the zero input).
 *   unsigned __gj_u32_clz_u  (alias)
 *   __libcgj_batch2287_marker = "libcgj-batch2287"
 *
 * Post-2280 bitops exclusive wave (2281-2290). Distinct from
 * gj_clz_u32 (batch788), gj_clz64 (batch367) - unique gj_u32_clz_u
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2287_marker[] = "libcgj-batch2287";

/* ---- freestanding helpers ---------------------------------------------- */

/* Binary-search peel of 16/8/4/2/1 against high half-masks. Zero -> 32. */
static unsigned
b2287_clz32(uint32_t u32X)
{
	unsigned uClz;
	uint32_t u32V;

	if (u32X == 0u) {
		return 32u;
	}

	u32V = u32X;
	uClz = 0u;

	if ((u32V & 0xffff0000u) == 0u) {
		uClz += 16u;
		u32V <<= 16;
	}
	if ((u32V & 0xff000000u) == 0u) {
		uClz += 8u;
		u32V <<= 8;
	}
	if ((u32V & 0xf0000000u) == 0u) {
		uClz += 4u;
		u32V <<= 4;
	}
	if ((u32V & 0xc0000000u) == 0u) {
		uClz += 2u;
		u32V <<= 2;
	}
	if ((u32V & 0x80000000u) == 0u) {
		uClz += 1u;
	}

	return uClz;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_clz_u - count leading zeros in a 32-bit word.
 *
 * x: value to inspect
 *
 * Returns consecutive zero count from MSB. x == 0 -> 32.
 * Does not call libc.
 */
unsigned
gj_u32_clz_u(uint32_t u32X)
{
	(void)NULL;
	return b2287_clz32(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u32_clz_u(uint32_t u32X)
    __attribute__((alias("gj_u32_clz_u")));
