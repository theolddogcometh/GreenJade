/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3565: find first (least-significant) set bit
 * in a uint64_t (_u).
 *
 * Surface (unique symbols):
 *   unsigned gj_u64_find_first_set_u(uint64_t x);
 *     - 1-based index of the lowest set bit (bit 0 → 1, bit 63 → 64).
 *       Returns 0 when x == 0 (no bits set). Classic ffs semantics.
 *   unsigned __gj_u64_find_first_set_u  (alias)
 *   __libcgj_batch3565_marker = "libcgj-batch3565"
 *
 * Exclusive continuum CREATE-ONLY (3561-3570). Companion to
 * gj_u32_find_first_set_u (batch3564). Distinct from gj_u64_ctz_u
 * (batch2286) — unique 1-based find_first_set surface only; no
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3565_marker[] = "libcgj-batch3565";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1-based index of lowest set bit. Zero input → 0.
 * Binary-search peel of trailing zeros then +1.
 */
static unsigned
b3565_ffs64(uint64_t u64X)
{
	unsigned uCtz;
	uint64_t u64V;

	if (u64X == 0ull) {
		return 0u;
	}

	u64V = u64X;
	uCtz = 0u;

	if ((u64V & 0xffffffffull) == 0ull) {
		uCtz += 32u;
		u64V >>= 32;
	}
	if ((u64V & 0xffffull) == 0ull) {
		uCtz += 16u;
		u64V >>= 16;
	}
	if ((u64V & 0xffull) == 0ull) {
		uCtz += 8u;
		u64V >>= 8;
	}
	if ((u64V & 0xfull) == 0ull) {
		uCtz += 4u;
		u64V >>= 4;
	}
	if ((u64V & 0x3ull) == 0ull) {
		uCtz += 2u;
		u64V >>= 2;
	}
	if ((u64V & 0x1ull) == 0ull) {
		uCtz += 1u;
	}

	return uCtz + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_find_first_set_u - 1-based index of lowest set bit.
 *
 * x: value to inspect
 *
 * Returns 1..64 for non-zero x (bit position of least-significant 1),
 * or 0 when x == 0. Does not call libc. No parent wires.
 */
unsigned
gj_u64_find_first_set_u(uint64_t u64X)
{
	(void)NULL;
	return b3565_ffs64(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u64_find_first_set_u(uint64_t u64X)
    __attribute__((alias("gj_u64_find_first_set_u")));
