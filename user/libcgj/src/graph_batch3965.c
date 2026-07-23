/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3965: 64-bit Hamming distance (unsigned).
 *
 * Surface (unique symbols):
 *   unsigned gj_u64_hamming_dist_u(uint64_t a, uint64_t b);
 *     - Number of bit positions that differ between a and b (0..64).
 *       Equal to popcount(a ^ b). SWAR path; no compiler builtins.
 *   unsigned __gj_u64_hamming_dist_u  (alias)
 *   __libcgj_batch3965_marker = "libcgj-batch3965"
 *
 * Exclusive continuum CREATE-ONLY (3961-3970). Distinct from
 * gj_hamming_distance (batch184, buffer), gj_u32_hamming_dist_u
 * (batch3964), and gj_u64_hamming_wt_u (batch3963) — unique
 * gj_u64_hamming_dist_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3965_marker[] = "libcgj-batch3965";

/* ---- freestanding helpers ---------------------------------------------- */

/* SWAR Hamming weight of one uint64_t (Hacker's Delight style). */
static unsigned
b3965_pop64(uint64_t u64X)
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
 * gj_u64_hamming_dist_u - Hamming distance between two uint64_t words.
 *
 * a, b: values to compare
 *
 * Returns popcount(a ^ b) in [0, 64]. a == b -> 0.
 * Does not call libc.
 */
unsigned
gj_u64_hamming_dist_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b3965_pop64(u64A ^ u64B);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u64_hamming_dist_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_hamming_dist_u")));
