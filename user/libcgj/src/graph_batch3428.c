/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3428: 64-bit population count (popcnt).
 *
 * Surface (unique symbols):
 *   unsigned gj_u64_popcnt_u(uint64_t x);
 *     - Number of 1-bits in a 64-bit word (0..64). SWAR path; no
 *       compiler builtins / libgcc popcount dependency.
 *   unsigned __gj_u64_popcnt_u  (alias)
 *   __libcgj_batch3428_marker = "libcgj-batch3428"
 *
 * Bitops exclusive CREATE-ONLY wave (3421-3430). Distinct from
 * gj_u64_popcount_u (batch2284), gj_popcnt_u64 (batch562),
 * gj_popcount_u64_u (batch2733) — unique gj_u64_popcnt_u surface
 * only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3428_marker[] = "libcgj-batch3428";

/* ---- freestanding helpers ---------------------------------------------- */

/* SWAR Hamming weight of one uint64_t (Hacker's Delight style). */
static unsigned
b3428_pop64(uint64_t u64X)
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
 * gj_u64_popcnt_u - population count of a 64-bit word.
 *
 * x: value to inspect
 *
 * Returns the number of set bits in [0, 64]. x == 0 -> 0.
 * Does not call libc. No parent wires.
 */
unsigned
gj_u64_popcnt_u(uint64_t u64X)
{
	(void)NULL;
	return b3428_pop64(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u64_popcnt_u(uint64_t u64X)
    __attribute__((alias("gj_u64_popcnt_u")));
