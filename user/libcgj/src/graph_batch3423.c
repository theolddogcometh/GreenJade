/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3423: leading zero count (lzcnt, u32).
 *
 * Surface (unique symbols):
 *   unsigned gj_u32_lzcnt_u(uint32_t x);
 *     - Number of consecutive 0-bits starting from bit 31 (MSB).
 *       Returns 32 when x == 0 (Intel LZCNT-compatible zero case).
 *   unsigned __gj_u32_lzcnt_u  (alias)
 *   __libcgj_batch3423_marker = "libcgj-batch3423"
 *
 * Bitops exclusive CREATE-ONLY wave (3421-3430). Distinct from
 * gj_u32_clz_u (batch2287), gj_clz_u32 (batch788) — unique
 * gj_u32_lzcnt_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3423_marker[] = "libcgj-batch3423";

/* ---- freestanding helpers ---------------------------------------------- */

/* Binary-search peel of 16/8/4/2/1 against high half-masks. Zero -> 32. */
static unsigned
b3423_lzcnt32(uint32_t u32X)
{
	unsigned uLz;
	uint32_t u32V;

	if (u32X == 0u) {
		return 32u;
	}

	u32V = u32X;
	uLz = 0u;

	if ((u32V & 0xffff0000u) == 0u) {
		uLz += 16u;
		u32V <<= 16;
	}
	if ((u32V & 0xff000000u) == 0u) {
		uLz += 8u;
		u32V <<= 8;
	}
	if ((u32V & 0xf0000000u) == 0u) {
		uLz += 4u;
		u32V <<= 4;
	}
	if ((u32V & 0xc0000000u) == 0u) {
		uLz += 2u;
		u32V <<= 2;
	}
	if ((u32V & 0x80000000u) == 0u) {
		uLz += 1u;
	}

	return uLz;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_lzcnt_u - count leading zeros in a 32-bit word (lzcnt style).
 *
 * x: value to inspect
 *
 * Returns consecutive zero count from MSB. x == 0 -> 32.
 * Does not call libc. No parent wires.
 */
unsigned
gj_u32_lzcnt_u(uint32_t u32X)
{
	(void)NULL;
	return b3423_lzcnt32(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u32_lzcnt_u(uint32_t u32X)
    __attribute__((alias("gj_u32_lzcnt_u")));
