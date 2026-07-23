/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3424: leading zero count (lzcnt, u64).
 *
 * Surface (unique symbols):
 *   unsigned gj_u64_lzcnt_u(uint64_t x);
 *     - Number of consecutive 0-bits starting from bit 63 (MSB).
 *       Returns 64 when x == 0 (Intel LZCNT-compatible zero case).
 *   unsigned __gj_u64_lzcnt_u  (alias)
 *   __libcgj_batch3424_marker = "libcgj-batch3424"
 *
 * Bitops exclusive CREATE-ONLY wave (3421-3430). Distinct from
 * gj_u64_clz_u (batch2288), gj_clz_u64 (batch564) — unique
 * gj_u64_lzcnt_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3424_marker[] = "libcgj-batch3424";

/* ---- freestanding helpers ---------------------------------------------- */

/* Binary-search peel of 32/16/8/4/2/1 against high half-masks. Zero -> 64. */
static unsigned
b3424_lzcnt64(uint64_t u64X)
{
	unsigned uLz;
	uint64_t u64V;

	if (u64X == 0ull) {
		return 64u;
	}

	u64V = u64X;
	uLz = 0u;

	if ((u64V & 0xffffffff00000000ull) == 0ull) {
		uLz += 32u;
		u64V <<= 32;
	}
	if ((u64V & 0xffff000000000000ull) == 0ull) {
		uLz += 16u;
		u64V <<= 16;
	}
	if ((u64V & 0xff00000000000000ull) == 0ull) {
		uLz += 8u;
		u64V <<= 8;
	}
	if ((u64V & 0xf000000000000000ull) == 0ull) {
		uLz += 4u;
		u64V <<= 4;
	}
	if ((u64V & 0xc000000000000000ull) == 0ull) {
		uLz += 2u;
		u64V <<= 2;
	}
	if ((u64V & 0x8000000000000000ull) == 0ull) {
		uLz += 1u;
	}

	return uLz;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_lzcnt_u - count leading zeros in a 64-bit word (lzcnt style).
 *
 * x: value to inspect
 *
 * Returns consecutive zero count from MSB. x == 0 -> 64.
 * Does not call libc. No parent wires.
 */
unsigned
gj_u64_lzcnt_u(uint64_t u64X)
{
	(void)NULL;
	return b3424_lzcnt64(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u64_lzcnt_u(uint64_t u64X)
    __attribute__((alias("gj_u64_lzcnt_u")));
