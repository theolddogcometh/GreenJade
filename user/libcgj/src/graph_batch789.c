/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch789: 32-bit population parity.
 *
 * Surface (unique symbols):
 *   int gj_parity_u32(uint32_t x);
 *     — Return 0 if x has even popcount, 1 if odd.
 *       XOR-fold pure integer path; no builtins.
 *   int __gj_parity_u32  (alias)
 *   __libcgj_batch789_marker = "libcgj-batch789"
 *
 * Distinct from gj_parity64 (batch419) — unique u32 surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch789_marker[] = "libcgj-batch789";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_parity_u32 — parity of set bits in a 32-bit word.
 *
 * Returns 0 for even popcount (including x == 0), 1 for odd.
 * Successive XOR-folds collapse to the LSB.
 */
int
gj_parity_u32(uint32_t u32X)
{
	uint32_t u32V;

	u32V = u32X;
	u32V ^= u32V >> 16;
	u32V ^= u32V >> 8;
	u32V ^= u32V >> 4;
	u32V ^= u32V >> 2;
	u32V ^= u32V >> 1;
	return (int)(u32V & 1u);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_parity_u32(uint32_t u32X)
    __attribute__((alias("gj_parity_u32")));
