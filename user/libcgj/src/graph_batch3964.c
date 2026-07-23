/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3964: 32-bit Hamming distance (unsigned).
 *
 * Surface (unique symbols):
 *   unsigned gj_u32_hamming_dist_u(uint32_t a, uint32_t b);
 *     - Number of bit positions that differ between a and b (0..32).
 *       Equal to popcount(a ^ b). SWAR path; no compiler builtins.
 *   unsigned __gj_u32_hamming_dist_u  (alias)
 *   __libcgj_batch3964_marker = "libcgj-batch3964"
 *
 * Exclusive continuum CREATE-ONLY (3961-3970). Distinct from
 * gj_hamming_distance (batch184, buffer), gj_hamming_u8 (batch537),
 * and gj_u32_hamming_wt_u (batch3962) — unique gj_u32_hamming_dist_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3964_marker[] = "libcgj-batch3964";

/* ---- freestanding helpers ---------------------------------------------- */

/* SWAR Hamming weight of one uint32_t (Hacker's Delight style). */
static unsigned
b3964_pop32(uint32_t u32X)
{
	u32X = u32X - ((u32X >> 1) & 0x55555555u);
	u32X = (u32X & 0x33333333u) + ((u32X >> 2) & 0x33333333u);
	u32X = (u32X + (u32X >> 4)) & 0x0f0f0f0fu;
	u32X = u32X + (u32X >> 8);
	u32X = u32X + (u32X >> 16);
	return (unsigned)(u32X & 0x3fu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_hamming_dist_u - Hamming distance between two uint32_t words.
 *
 * a, b: values to compare
 *
 * Returns popcount(a ^ b) in [0, 32]. a == b -> 0.
 * Does not call libc.
 */
unsigned
gj_u32_hamming_dist_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b3964_pop32(u32A ^ u32B);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u32_hamming_dist_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_hamming_dist_u")));
