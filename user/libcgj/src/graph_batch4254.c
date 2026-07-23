/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4254: 64-bit Hamming distance (XOR popcount).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u64_hamming_u(uint64_t a, uint64_t b);
 *     - Return popcount(a ^ b): bit-level Hamming distance of two
 *       64-bit words. SWAR pure integer path; no builtins.
 *   uint32_t __gj_u64_hamming_u  (alias)
 *   __libcgj_batch4254_marker = "libcgj-batch4254"
 *
 * Exclusive continuum CREATE-ONLY (4251-4260). Unique gj_u64_hamming_u
 * surface only; no multi-def. Distinct from gj_u32_hamming_u
 * (batch4253) and buffer Hamming paths. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4254_marker[] = "libcgj-batch4254";

/* ---- freestanding helpers ---------------------------------------------- */

/* SWAR popcount of a 64-bit word (no __builtin dependency). */
static uint32_t
b4254_pop64(uint64_t u64X)
{
	uint64_t u64V;

	u64V = u64X;
	u64V = u64V - ((u64V >> 1) & 0x5555555555555555ull);
	u64V = (u64V & 0x3333333333333333ull) +
	    ((u64V >> 2) & 0x3333333333333333ull);
	u64V = (u64V + (u64V >> 4)) & 0x0f0f0f0f0f0f0f0full;
	u64V = u64V + (u64V >> 8);
	u64V = u64V + (u64V >> 16);
	u64V = u64V + (u64V >> 32);
	return (uint32_t)(u64V & 0x7full);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_hamming_u - Hamming distance between two 64-bit words.
 *
 * a, b: words to compare
 *
 * Returns popcount(a ^ b) in [0, 64]. Does not call libc. No parent wires.
 */
uint32_t
gj_u64_hamming_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b4254_pop64(u64A ^ u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u64_hamming_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_hamming_u")));
