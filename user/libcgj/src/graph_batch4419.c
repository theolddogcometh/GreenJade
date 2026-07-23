/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4419: one-hot uint32_t to binary bit index.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_onehot_to_bin_u(uint32_t x);
 *     - If x is a power of two (exactly one bit set), return the ctz
 *       index of that bit; otherwise return 0xffffffffu.
 *   uint32_t __gj_u32_onehot_to_bin_u  (alias)
 *   __libcgj_batch4419_marker = "libcgj-batch4419"
 *
 * Exclusive continuum CREATE-ONLY (4411-4420: gray code unique). Unique
 * gj_u32_onehot_to_bin_u surface only; no multi-def. Complements
 * gj_u32_bin_to_onehot_u (batch4418). Freestanding ctz (no builtins).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4419_marker[] = "libcgj-batch4419";

/* ---- freestanding helpers ---------------------------------------------- */

/* Count trailing zeros for nonzero x (binary-search peel). */
static uint32_t
b4419_ctz32(uint32_t u32X)
{
	uint32_t uCtz;
	uint32_t u32V;

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

	return uCtz;
}

/* True iff x has exactly one bit set. */
static int
b4419_is_pow2(uint32_t u32X)
{
	return (u32X != 0u) && ((u32X & (u32X - 1u)) == 0u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_onehot_to_bin_u - decode a one-hot mask to its bit index.
 *
 * x: candidate one-hot word
 *
 * Returns ctz(x) when x is a power of two; otherwise 0xffffffffu
 * (including x == 0 and multi-bit masks). No parent wires.
 */
uint32_t
gj_u32_onehot_to_bin_u(uint32_t u32X)
{
	(void)NULL;
	if (!b4419_is_pow2(u32X)) {
		return 0xffffffffu;
	}
	return b4419_ctz32(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_onehot_to_bin_u(uint32_t u32X)
    __attribute__((alias("gj_u32_onehot_to_bin_u")));
