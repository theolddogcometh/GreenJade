/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3566: find last (most-significant) set bit
 * in a uint32_t (_u).
 *
 * Surface (unique symbols):
 *   unsigned gj_u32_find_last_set_u(uint32_t x);
 *     - 1-based index of the highest set bit (bit 0 → 1, bit 31 → 32).
 *       Returns 0 when x == 0 (no bits set). Classic fls semantics.
 *   unsigned __gj_u32_find_last_set_u  (alias)
 *   __libcgj_batch3566_marker = "libcgj-batch3566"
 *
 * Exclusive continuum CREATE-ONLY (3561-3570). Distinct from
 * gj_u32_clz_u (batch2287 leading-zero count) — unique 1-based
 * find_last_set surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3566_marker[] = "libcgj-batch3566";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1-based index of highest set bit. Zero input → 0.
 * Binary-search peel of leading zeros then fls = 32 - clz.
 */
static unsigned
b3566_fls32(uint32_t u32X)
{
	unsigned uClz;
	uint32_t u32V;

	if (u32X == 0u) {
		return 0u;
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

	return 32u - uClz;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_find_last_set_u - 1-based index of highest set bit.
 *
 * x: value to inspect
 *
 * Returns 1..32 for non-zero x (bit position of most-significant 1),
 * or 0 when x == 0. Does not call libc. No parent wires.
 */
unsigned
gj_u32_find_last_set_u(uint32_t u32X)
{
	(void)NULL;
	return b3566_fls32(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u32_find_last_set_u(uint32_t u32X)
    __attribute__((alias("gj_u32_find_last_set_u")));
