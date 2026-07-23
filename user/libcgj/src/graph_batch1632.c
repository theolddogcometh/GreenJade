/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1632: single-word bitset parity (u64).
 *
 * Surface (unique symbols):
 *   int gj_bitset64_parity(uint64_t x);
 *     — Return 0 if x has even popcount, 1 if odd. XOR-fold pure
 *       integer path; no builtins.
 *   int __gj_bitset64_parity  (alias)
 *   __libcgj_batch1632_marker = "libcgj-batch1632"
 *
 * Distinct from gj_parity64 (batch419), gj_parity_u64 (batch595), and
 * gj_parity_u32 (batch789) — unique bitset64 surface.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Self-contained.
 */

#include <stdint.h>

const char __libcgj_batch1632_marker[] = "libcgj-batch1632";

/* ---- freestanding helpers ---------------------------------------------- */

/* Fold popcount parity of u64X into LSB via successive XOR halves. */
static int
b1632_parity(uint64_t u64X)
{
	uint64_t u64V;

	u64V = u64X;
	u64V ^= u64V >> 32;
	u64V ^= u64V >> 16;
	u64V ^= u64V >> 8;
	u64V ^= u64V >> 4;
	u64V ^= u64V >> 2;
	u64V ^= u64V >> 1;
	return (int)(u64V & 1ull);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitset64_parity — parity of set bits in a 64-bit bitset word.
 *
 * x: dense 64-bit bitset (bit 0 = LSB).
 *
 * Returns 0 for even popcount (including x == 0), 1 for odd.
 */
int
gj_bitset64_parity(uint64_t u64X)
{
	return b1632_parity(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bitset64_parity(uint64_t u64X)
    __attribute__((alias("gj_bitset64_parity")));
