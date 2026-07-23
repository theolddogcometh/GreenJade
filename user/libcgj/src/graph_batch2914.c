/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2914: Hamming distance of two uint32_t words
 * (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hamming_u32_u(uint32_t a, uint32_t b);
 *     - Popcount of (a XOR b); number of differing bits in [0, 32].
 *   uint32_t __gj_hamming_u32_u  (alias)
 *   __libcgj_batch2914_marker = "libcgj-batch2914"
 *
 * Exclusive continuum wave 2911-2920. Distinct from gj_hamming_distance
 * (batch184), gj_hamming_u8 (batch537), gj_popcount32 (batch139).
 * Local SWAR popcount; no parent wires. Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no __builtin_popcount. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2914_marker[] = "libcgj-batch2914";

/* ---- freestanding helpers ---------------------------------------------- */

/* SWAR Hamming weight of a 32-bit word (0..32). */
static uint32_t
b2914_pop32(uint32_t u32X)
{
	u32X = u32X - ((u32X >> 1) & 0x55555555u);
	u32X = (u32X & 0x33333333u) + ((u32X >> 2) & 0x33333333u);
	u32X = (u32X + (u32X >> 4)) & 0x0f0f0f0fu;
	u32X = u32X + (u32X >> 8);
	u32X = u32X + (u32X >> 16);
	return u32X & 0x3fu;
}

static uint32_t
b2914_hamming(uint32_t u32A, uint32_t u32B)
{
	return b2914_pop32(u32A ^ u32B);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hamming_u32_u - bit Hamming distance between two 32-bit words.
 *
 * a, b: words to compare
 *
 * Returns popcount(a ^ b) in 0..32. Does not call libc or parent gj_*.
 */
uint32_t
gj_hamming_u32_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b2914_hamming(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hamming_u32_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_hamming_u32_u")));
