/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3426: trailing zero count (tzcnt, u64).
 *
 * Surface (unique symbols):
 *   unsigned gj_u64_tzcnt_u(uint64_t x);
 *     - Number of consecutive 0-bits starting from bit 0 (LSB).
 *       Returns 64 when x == 0 (Intel TZCNT-compatible zero case).
 *   unsigned __gj_u64_tzcnt_u  (alias)
 *   __libcgj_batch3426_marker = "libcgj-batch3426"
 *
 * Bitops exclusive CREATE-ONLY wave (3421-3430). Distinct from
 * gj_u64_ctz_u (batch2286), gj_ctz_u64 (batch563) — unique
 * gj_u64_tzcnt_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3426_marker[] = "libcgj-batch3426";

/* ---- freestanding helpers ---------------------------------------------- */

/* Binary-search peel 32/16/8/4/2/1 with masks. Zero input -> 64. */
static unsigned
b3426_tzcnt64(uint64_t u64X)
{
	unsigned uTz;
	uint64_t u64V;

	if (u64X == 0ull) {
		return 64u;
	}

	u64V = u64X;
	uTz = 0u;

	if ((u64V & 0xffffffffull) == 0ull) {
		uTz += 32u;
		u64V >>= 32;
	}
	if ((u64V & 0xffffull) == 0ull) {
		uTz += 16u;
		u64V >>= 16;
	}
	if ((u64V & 0xffull) == 0ull) {
		uTz += 8u;
		u64V >>= 8;
	}
	if ((u64V & 0xfull) == 0ull) {
		uTz += 4u;
		u64V >>= 4;
	}
	if ((u64V & 0x3ull) == 0ull) {
		uTz += 2u;
		u64V >>= 2;
	}
	if ((u64V & 0x1ull) == 0ull) {
		uTz += 1u;
	}

	return uTz;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_tzcnt_u - count trailing zeros in a 64-bit word (tzcnt style).
 *
 * x: value to inspect
 *
 * Returns consecutive zero count from LSB. x == 0 -> 64.
 * Does not call libc. No parent wires.
 */
unsigned
gj_u64_tzcnt_u(uint64_t u64X)
{
	(void)NULL;
	return b3426_tzcnt64(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u64_tzcnt_u(uint64_t u64X)
    __attribute__((alias("gj_u64_tzcnt_u")));
