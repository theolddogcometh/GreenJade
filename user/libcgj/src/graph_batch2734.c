/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2734: count trailing zeros in a uint32_t (_u).
 *
 * Surface (unique symbols):
 *   unsigned gj_ctz_u32_u(uint32_t x);
 *     - Number of consecutive 0-bits starting from bit 0 (LSB).
 *       Returns 32 when x == 0 (fully defined for the zero input).
 *   unsigned __gj_ctz_u32_u  (alias)
 *   __libcgj_batch2734_marker = "libcgj-batch2734"
 *
 * Bitops exclusive wave (2731-2740). Distinct from gj_ctz_u32
 * (batch787), gj_u32_ctz_u (batch2285), gj_ctz64 (batch366) — unique
 * gj_ctz_u32_u surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2734_marker[] = "libcgj-batch2734";

/* ---- freestanding helpers ---------------------------------------------- */

/* Binary-search peel of 16/8/4/2/1 against low masks. Zero -> 32. */
static unsigned
b2734_ctz32(uint32_t u32X)
{
	unsigned uCtz;
	uint32_t u32V;

	if (u32X == 0u) {
		return 32u;
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

	return uCtz;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ctz_u32_u - count trailing zeros in a 32-bit word.
 *
 * x: value to inspect
 *
 * Returns consecutive zero count from LSB. x == 0 -> 32.
 * Does not call libc.
 */
unsigned
gj_ctz_u32_u(uint32_t u32X)
{
	(void)NULL;
	return b2734_ctz32(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_ctz_u32_u(uint32_t u32X)
    __attribute__((alias("gj_ctz_u32_u")));
