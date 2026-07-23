/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2733: 64-bit population count (_u continuum).
 *
 * Surface (unique symbols):
 *   unsigned gj_popcount_u64_u(uint64_t x);
 *     - Number of 1-bits in a 64-bit word (0..64). SWAR scalar path.
 *   unsigned __gj_popcount_u64_u  (alias)
 *   __libcgj_batch2733_marker = "libcgj-batch2733"
 *
 * Bitops exclusive wave (2731-2740). Distinct from gj_popcount64
 * (batch139) — unique gj_popcount_u64_u surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2733_marker[] = "libcgj-batch2733";

/* ---- freestanding helpers ---------------------------------------------- */

/* Hamming weight via parallel bit sums (SWAR). */
static unsigned
b2733_pop64(uint64_t u64X)
{
	uint64_t x;

	x = u64X;
	x = x - ((x >> 1) & 0x5555555555555555ull);
	x = (x & 0x3333333333333333ull) + ((x >> 2) & 0x3333333333333333ull);
	x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0full;
	x = x + (x >> 8);
	x = x + (x >> 16);
	x = x + (x >> 32);
	return (unsigned)(x & 0x7full);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_popcount_u64_u - count set bits in a 64-bit word.
 *
 * x: value to inspect
 *
 * Returns Hamming weight in 0..64. Does not call libc or builtins.
 */
unsigned
gj_popcount_u64_u(uint64_t u64X)
{
	(void)NULL;
	return b2733_pop64(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_popcount_u64_u(uint64_t u64X)
    __attribute__((alias("gj_popcount_u64_u")));
