/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4252: 64-bit population parity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u64_pop_parity_u(uint64_t x);
 *     - Return popcount(x) & 1: 0 if even popcount, 1 if odd.
 *       XOR-fold pure integer path; no builtins.
 *   uint32_t __gj_u64_pop_parity_u  (alias)
 *   __libcgj_batch4252_marker = "libcgj-batch4252"
 *
 * Exclusive continuum CREATE-ONLY (4251-4260). Unique
 * gj_u64_pop_parity_u surface only; no multi-def. Distinct from
 * gj_u32_pop_parity_u (batch4251) and gj_parity64 (batch419). No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4252_marker[] = "libcgj-batch4252";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Population parity of a 64-bit word via successive XOR folds.
 * Equivalent to popcount(x) & 1 without a full SWAR popcount.
 */
static uint32_t
b4252_pop_parity(uint64_t u64X)
{
	uint64_t u64V;

	u64V = u64X;
	u64V ^= u64V >> 32;
	u64V ^= u64V >> 16;
	u64V ^= u64V >> 8;
	u64V ^= u64V >> 4;
	u64V ^= u64V >> 2;
	u64V ^= u64V >> 1;
	return (uint32_t)(u64V & 1ull);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_pop_parity_u - parity of set bits in a 64-bit word.
 *
 * x: value to inspect
 *
 * Returns 0 for even popcount (including x == 0), 1 for odd.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_u64_pop_parity_u(uint64_t u64X)
{
	(void)NULL;
	return b4252_pop_parity(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u64_pop_parity_u(uint64_t u64X)
    __attribute__((alias("gj_u64_pop_parity_u")));
