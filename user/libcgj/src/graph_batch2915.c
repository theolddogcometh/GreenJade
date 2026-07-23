/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2915: Hamming distance of two uint64_t words
 * (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hamming_u64_u(uint64_t a, uint64_t b);
 *     - Popcount of (a XOR b); number of differing bits in [0, 64].
 *   uint32_t __gj_hamming_u64_u  (alias)
 *   __libcgj_batch2915_marker = "libcgj-batch2915"
 *
 * Exclusive continuum wave 2911-2920. Distinct from gj_hamming_u32_u
 * (batch2914), gj_hamming_distance (batch184), gj_popcount64 (batch139).
 * Local SWAR popcount; no parent wires. Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no __builtin_popcountll. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2915_marker[] = "libcgj-batch2915";

/* ---- freestanding helpers ---------------------------------------------- */

/* SWAR Hamming weight of a 64-bit word (0..64). */
static uint32_t
b2915_pop64(uint64_t u64X)
{
	u64X = u64X - ((u64X >> 1) & 0x5555555555555555ULL);
	u64X = (u64X & 0x3333333333333333ULL) + ((u64X >> 2) & 0x3333333333333333ULL);
	u64X = (u64X + (u64X >> 4)) & 0x0f0f0f0f0f0f0f0fULL;
	u64X = u64X + (u64X >> 8);
	u64X = u64X + (u64X >> 16);
	u64X = u64X + (u64X >> 32);
	return (uint32_t)(u64X & 0x7fULL);
}

static uint32_t
b2915_hamming(uint64_t u64A, uint64_t u64B)
{
	return b2915_pop64(u64A ^ u64B);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hamming_u64_u - bit Hamming distance between two 64-bit words.
 *
 * a, b: words to compare
 *
 * Returns popcount(a ^ b) in 0..64. Does not call libc or parent gj_*.
 */
uint32_t
gj_hamming_u64_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b2915_hamming(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hamming_u64_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_hamming_u64_u")));
