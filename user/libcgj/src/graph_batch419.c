/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch419: 64-bit population parity.
 *
 * Surface (unique symbols):
 *   int gj_parity64(uint64_t x);
 *     — Return 0 if x has even popcount, 1 if odd.
 *       XOR-fold pure integer path; no builtins.
 *   int __gj_parity64  (alias)
 *   __libcgj_batch419_marker = "libcgj-batch419"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch419_marker[] = "libcgj-batch419";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_parity64 — parity of set bits in a 64-bit word.
 *
 * x: value to inspect
 *
 * Returns 0 for even popcount (including x == 0), 1 for odd.
 * Successive XOR-folds collapse to the LSB.
 */
int
gj_parity64(uint64_t u64X)
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

/* ---- underscored alias ------------------------------------------------- */

int __gj_parity64(uint64_t u64X)
    __attribute__((alias("gj_parity64")));
