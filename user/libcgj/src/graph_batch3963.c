/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3963: 64-bit Hamming weight (unsigned).
 *
 * Surface (unique symbols):
 *   unsigned gj_u64_hamming_wt_u(uint64_t x);
 *     - Number of 1-bits in a 64-bit word (0..64). SWAR path; no
 *       compiler builtins / libgcc popcount dependency.
 *   unsigned __gj_u64_hamming_wt_u  (alias)
 *   __libcgj_batch3963_marker = "libcgj-batch3963"
 *
 * Exclusive continuum CREATE-ONLY (3961-3970). Distinct from
 * gj_u64_popcount_u (batch2284), gj_popcount64 (batch139), and
 * gj_u32_hamming_wt_u (batch3962) — unique gj_u64_hamming_wt_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3963_marker[] = "libcgj-batch3963";

/* ---- freestanding helpers ---------------------------------------------- */

/* SWAR Hamming weight of one uint64_t (Hacker's Delight style). */
static unsigned
b3963_pop64(uint64_t u64X)
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
 * gj_u64_hamming_wt_u - Hamming weight (population count) of a uint64_t.
 *
 * x: value to inspect
 *
 * Returns the number of set bits in [0, 64]. x == 0 -> 0.
 * Does not call libc.
 */
unsigned
gj_u64_hamming_wt_u(uint64_t u64X)
{
	(void)NULL;
	return b3963_pop64(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u64_hamming_wt_u(uint64_t u64X)
    __attribute__((alias("gj_u64_hamming_wt_u")));
