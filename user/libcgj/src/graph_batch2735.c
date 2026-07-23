/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2735: count trailing zeros in a uint64_t (_u).
 *
 * Surface (unique symbols):
 *   unsigned gj_ctz_u64_u(uint64_t x);
 *     - Number of consecutive 0-bits starting from bit 0 (LSB).
 *       Returns 64 when x == 0 (fully defined for the zero input).
 *   unsigned __gj_ctz_u64_u  (alias)
 *   __libcgj_batch2735_marker = "libcgj-batch2735"
 *
 * Bitops exclusive wave (2731-2740). Distinct from gj_ctz_u64
 * (batch563), gj_ctz64 (batch366, zero -> 0) — unique gj_ctz_u64_u
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2735_marker[] = "libcgj-batch2735";

/* ---- freestanding helpers ---------------------------------------------- */

/* Binary-search peel of 32/16/8/4/2/1 against low masks. Zero -> 64. */
static unsigned
b2735_ctz64(uint64_t u64X)
{
	unsigned uCtz;
	uint64_t u64V;

	if (u64X == 0ull) {
		return 64u;
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

	return uCtz;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ctz_u64_u - count trailing zeros in a 64-bit word.
 *
 * x: value to inspect
 *
 * Returns consecutive zero count from LSB. x == 0 -> 64.
 * Does not call libc.
 */
unsigned
gj_ctz_u64_u(uint64_t u64X)
{
	(void)NULL;
	return b2735_ctz64(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_ctz_u64_u(uint64_t u64X)
    __attribute__((alias("gj_ctz_u64_u")));
