/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2284: 64-bit population count (unsigned).
 *
 * Surface (unique symbols):
 *   unsigned gj_u64_popcount_u(uint64_t x);
 *     - Number of 1-bits in a 64-bit word (0..64). SWAR path; no
 *       compiler builtins / libgcc popcount dependency.
 *   unsigned __gj_u64_popcount_u  (alias)
 *   __libcgj_batch2284_marker = "libcgj-batch2284"
 *
 * Post-2280 bitops exclusive wave (2281-2290). Distinct from
 * gj_popcount64 (batch139), gj_bitset64_popcount (batch1631) - unique
 * gj_u64_popcount_u surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2284_marker[] = "libcgj-batch2284";

/* ---- freestanding helpers ---------------------------------------------- */

/* SWAR Hamming weight of one uint64_t (Hacker's Delight style). */
static unsigned
b2284_pop64(uint64_t u64X)
{
	u64X = u64X - ((u64X >> 1) & 0x5555555555555555ull);
	u64X = (u64X & 0x3333333333333333ull) +
	    ((u64X >> 2) & 0x3333333333333333ull);
	u64X = (u64X + (u64X >> 4)) & 0x0f0f0f0f0f0f0f0full;
	u64X = u64X + (u64X >> 8);
	u64X = u64X + (u64X >> 16);
	u64X = u64X + (u64X >> 32);
	return (unsigned)(u64X & 0x7full);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_popcount_u - population count of a 64-bit word.
 *
 * x: value to inspect
 *
 * Returns the number of set bits in [0, 64). x == 0 -> 0.
 * Does not call libc.
 */
unsigned
gj_u64_popcount_u(uint64_t u64X)
{
	(void)NULL;
	return b2284_pop64(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u64_popcount_u(uint64_t u64X)
    __attribute__((alias("gj_u64_popcount_u")));
