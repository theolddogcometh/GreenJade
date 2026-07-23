/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2286: count trailing zero bits in a uint64_t.
 *
 * Surface (unique symbols):
 *   unsigned gj_u64_ctz_u(uint64_t x);
 *     - Number of consecutive 0-bits starting from bit 0 (LSB).
 *       Returns 64 when x == 0 (full word width; matches common
 *       ctzll-on-zero conventions).
 *   unsigned __gj_u64_ctz_u  (alias)
 *   __libcgj_batch2286_marker = "libcgj-batch2286"
 *
 * Post-2280 bitops exclusive wave (2281-2290). Distinct from
 * gj_ctz_u64 (batch563), gj_ctz64 (batch366) - unique gj_u64_ctz_u
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2286_marker[] = "libcgj-batch2286";

/* ---- freestanding helpers ---------------------------------------------- */

/* Binary-search peel 32/16/8/4/2/1 with masks. Zero input -> 64. */
static unsigned
b2286_ctz64(uint64_t u64X)
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
 * gj_u64_ctz_u - count trailing zeros in a 64-bit word.
 *
 * x: value to inspect
 *
 * Returns consecutive zero count from LSB. x == 0 -> 64.
 * Does not call libc.
 */
unsigned
gj_u64_ctz_u(uint64_t u64X)
{
	(void)NULL;
	return b2286_ctz64(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u64_ctz_u(uint64_t u64X)
    __attribute__((alias("gj_u64_ctz_u")));
