/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2739: 64-bit population parity (_u continuum).
 *
 * Surface (unique symbols):
 *   unsigned gj_parity_u64_u(uint64_t x);
 *     - Return 0 if x has even popcount, 1 if odd (including x == 0 -> 0).
 *       XOR-fold pure integer path; no builtins.
 *   unsigned __gj_parity_u64_u  (alias)
 *   __libcgj_batch2739_marker = "libcgj-batch2739"
 *
 * Bitops exclusive wave (2731-2740). Distinct from gj_parity_u64
 * (batch595), gj_parity64 (batch419) — unique gj_parity_u64_u surface
 * only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2739_marker[] = "libcgj-batch2739";

/* ---- freestanding helpers ---------------------------------------------- */

/* Successive XOR-folds collapse to the LSB. */
static unsigned
b2739_parity64(uint64_t u64X)
{
	uint64_t u64V;

	u64V = u64X;
	u64V ^= u64V >> 32;
	u64V ^= u64V >> 16;
	u64V ^= u64V >> 8;
	u64V ^= u64V >> 4;
	u64V ^= u64V >> 2;
	u64V ^= u64V >> 1;
	return (unsigned)(u64V & 1ull);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_parity_u64_u - parity of set bits in a 64-bit word.
 *
 * x: value to inspect
 *
 * Returns 0 for even popcount (including x == 0), 1 for odd.
 * Does not call libc.
 */
unsigned
gj_parity_u64_u(uint64_t u64X)
{
	(void)NULL;
	return b2739_parity64(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_parity_u64_u(uint64_t u64X)
    __attribute__((alias("gj_parity_u64_u")));
