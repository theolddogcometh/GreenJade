/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3567: find last (most-significant) set bit
 * in a uint64_t (_u).
 *
 * Surface (unique symbols):
 *   unsigned gj_u64_find_last_set_u(uint64_t x);
 *     - 1-based index of the highest set bit (bit 0 → 1, bit 63 → 64).
 *       Returns 0 when x == 0 (no bits set). Classic fls semantics.
 *   unsigned __gj_u64_find_last_set_u  (alias)
 *   __libcgj_batch3567_marker = "libcgj-batch3567"
 *
 * Exclusive continuum CREATE-ONLY (3561-3570). Companion to
 * gj_u32_find_last_set_u (batch3566). Distinct from gj_u64_clz_u
 * (batch2288) — unique 1-based find_last_set surface only; no
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3567_marker[] = "libcgj-batch3567";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1-based index of highest set bit. Zero input → 0.
 * Binary-search peel of leading zeros then fls = 64 - clz.
 */
static unsigned
b3567_fls64(uint64_t u64X)
{
	unsigned uClz;
	uint64_t u64V;

	if (u64X == 0ull) {
		return 0u;
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

	return 64u - uClz;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_find_last_set_u - 1-based index of highest set bit.
 *
 * x: value to inspect
 *
 * Returns 1..64 for non-zero x (bit position of most-significant 1),
 * or 0 when x == 0. Does not call libc. No parent wires.
 */
unsigned
gj_u64_find_last_set_u(uint64_t u64X)
{
	(void)NULL;
	return b3567_fls64(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u64_find_last_set_u(uint64_t u64X)
    __attribute__((alias("gj_u64_find_last_set_u")));
