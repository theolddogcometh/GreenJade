/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3425: trailing zero count (tzcnt, u32).
 *
 * Surface (unique symbols):
 *   unsigned gj_u32_tzcnt_u(uint32_t x);
 *     - Number of consecutive 0-bits starting from bit 0 (LSB).
 *       Returns 32 when x == 0 (Intel TZCNT-compatible zero case).
 *   unsigned __gj_u32_tzcnt_u  (alias)
 *   __libcgj_batch3425_marker = "libcgj-batch3425"
 *
 * Bitops exclusive CREATE-ONLY wave (3421-3430). Distinct from
 * gj_u32_ctz_u (batch2285), gj_ctz_u32 (batch787) — unique
 * gj_u32_tzcnt_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3425_marker[] = "libcgj-batch3425";

/* ---- freestanding helpers ---------------------------------------------- */

/* Binary-search peel of 16/8/4/2/1 against low masks. Zero -> 32. */
static unsigned
b3425_tzcnt32(uint32_t u32X)
{
	unsigned uTz;
	uint32_t u32V;

	if (u32X == 0u) {
		return 32u;
	}

	u32V = u32X;
	uTz = 0u;

	if ((u32V & 0xffffu) == 0u) {
		uTz += 16u;
		u32V >>= 16;
	}
	if ((u32V & 0xffu) == 0u) {
		uTz += 8u;
		u32V >>= 8;
	}
	if ((u32V & 0xfu) == 0u) {
		uTz += 4u;
		u32V >>= 4;
	}
	if ((u32V & 0x3u) == 0u) {
		uTz += 2u;
		u32V >>= 2;
	}
	if ((u32V & 0x1u) == 0u) {
		uTz += 1u;
	}

	return uTz;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_tzcnt_u - count trailing zeros in a 32-bit word (tzcnt style).
 *
 * x: value to inspect
 *
 * Returns consecutive zero count from LSB. x == 0 -> 32.
 * Does not call libc. No parent wires.
 */
unsigned
gj_u32_tzcnt_u(uint32_t u32X)
{
	(void)NULL;
	return b3425_tzcnt32(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u32_tzcnt_u(uint32_t u32X)
    __attribute__((alias("gj_u32_tzcnt_u")));
