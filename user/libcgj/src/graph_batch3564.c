/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3564: find first (least-significant) set bit
 * in a uint32_t (_u).
 *
 * Surface (unique symbols):
 *   unsigned gj_u32_find_first_set_u(uint32_t x);
 *     - 1-based index of the lowest set bit (bit 0 → 1, bit 31 → 32).
 *       Returns 0 when x == 0 (no bits set). Classic ffs semantics.
 *   unsigned __gj_u32_find_first_set_u  (alias)
 *   __libcgj_batch3564_marker = "libcgj-batch3564"
 *
 * Exclusive continuum CREATE-ONLY (3561-3570). Distinct from
 * gj_u32_ctz_u (batch2285 0-based width-on-zero) — unique 1-based
 * find_first_set surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3564_marker[] = "libcgj-batch3564";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1-based index of lowest set bit. Zero input → 0.
 * Binary-search peel of trailing zeros then +1.
 */
static unsigned
b3564_ffs32(uint32_t u32X)
{
	unsigned uCtz;
	uint32_t u32V;

	if (u32X == 0u) {
		return 0u;
	}

	u32V = u32X;
	uCtz = 0u;

	if ((u32V & 0xffffu) == 0u) {
		uCtz += 16u;
		u32V >>= 16;
	}
	if ((u32V & 0xffu) == 0u) {
		uCtz += 8u;
		u32V >>= 8;
	}
	if ((u32V & 0xfu) == 0u) {
		uCtz += 4u;
		u32V >>= 4;
	}
	if ((u32V & 0x3u) == 0u) {
		uCtz += 2u;
		u32V >>= 2;
	}
	if ((u32V & 0x1u) == 0u) {
		uCtz += 1u;
	}

	return uCtz + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_find_first_set_u - 1-based index of lowest set bit.
 *
 * x: value to inspect
 *
 * Returns 1..32 for non-zero x (bit position of least-significant 1),
 * or 0 when x == 0. Does not call libc. No parent wires.
 */
unsigned
gj_u32_find_first_set_u(uint32_t u32X)
{
	(void)NULL;
	return b3564_ffs32(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u32_find_first_set_u(uint32_t u32X)
    __attribute__((alias("gj_u32_find_first_set_u")));
