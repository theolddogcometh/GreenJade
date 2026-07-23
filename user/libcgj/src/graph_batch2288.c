/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2288: count leading zero bits in a uint64_t.
 *
 * Surface (unique symbols):
 *   unsigned gj_u64_clz_u(uint64_t x);
 *     - Number of consecutive 0-bits starting from bit 63 (MSB).
 *       Returns 64 when x == 0 (fully defined for the zero input).
 *   unsigned __gj_u64_clz_u  (alias)
 *   __libcgj_batch2288_marker = "libcgj-batch2288"
 *
 * Post-2280 bitops exclusive wave (2281-2290). Distinct from
 * gj_clz_u64 (batch564), gj_clz64 (batch367) - unique gj_u64_clz_u
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2288_marker[] = "libcgj-batch2288";

/* ---- freestanding helpers ---------------------------------------------- */

/* Binary-search peel of 32/16/8/4/2/1 against high half-masks. Zero -> 64. */
static unsigned
b2288_clz64(uint64_t u64X)
{
	unsigned uClz;
	uint64_t u64V;

	if (u64X == 0ull) {
		return 64u;
	}

	u64V = u64X;
	uClz = 0u;

	if ((u64V & 0xffffffff00000000ull) == 0ull) {
		uClz += 32u;
		u64V <<= 32;
	}
	if ((u64V & 0xffff000000000000ull) == 0ull) {
		uClz += 16u;
		u64V <<= 16;
	}
	if ((u64V & 0xff00000000000000ull) == 0ull) {
		uClz += 8u;
		u64V <<= 8;
	}
	if ((u64V & 0xf000000000000000ull) == 0ull) {
		uClz += 4u;
		u64V <<= 4;
	}
	if ((u64V & 0xc000000000000000ull) == 0ull) {
		uClz += 2u;
		u64V <<= 2;
	}
	if ((u64V & 0x8000000000000000ull) == 0ull) {
		uClz += 1u;
	}

	return uClz;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_clz_u - count leading zeros in a 64-bit word.
 *
 * x: value to inspect
 *
 * Returns consecutive zero count from MSB. x == 0 -> 64.
 * Does not call libc.
 */
unsigned
gj_u64_clz_u(uint64_t u64X)
{
	(void)NULL;
	return b2288_clz64(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u64_clz_u(uint64_t u64X)
    __attribute__((alias("gj_u64_clz_u")));
