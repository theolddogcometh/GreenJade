/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4251: 32-bit population parity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_pop_parity_u(uint32_t x);
 *     - Return popcount(x) & 1: 0 if even popcount, 1 if odd.
 *       XOR-fold pure integer path; no builtins.
 *   uint32_t __gj_u32_pop_parity_u  (alias)
 *   __libcgj_batch4251_marker = "libcgj-batch4251"
 *
 * Exclusive continuum CREATE-ONLY (4251-4260): u32_pop_parity_u,
 * u64_pop_parity_u, u32_hamming_u, u64_hamming_u, u32_bit_deposit_low_u,
 * u32_bit_extract_low_u, u32_isolate_msb_u, u32_isolate_lsb_u,
 * u32_clear_lsb_u, batch_id_4260. Unique gj_u32_pop_parity_u surface
 * only; no multi-def. Distinct from gj_parity64 (batch419). No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4251_marker[] = "libcgj-batch4251";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Population parity of a 32-bit word via successive XOR folds.
 * Equivalent to popcount(x) & 1 without a full SWAR popcount.
 */
static uint32_t
b4251_pop_parity(uint32_t u32X)
{
	uint32_t u32V;

	u32V = u32X;
	u32V ^= u32V >> 16;
	u32V ^= u32V >> 8;
	u32V ^= u32V >> 4;
	u32V ^= u32V >> 2;
	u32V ^= u32V >> 1;
	return u32V & 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_pop_parity_u - parity of set bits in a 32-bit word.
 *
 * x: value to inspect
 *
 * Returns 0 for even popcount (including x == 0), 1 for odd.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_u32_pop_parity_u(uint32_t u32X)
{
	(void)NULL;
	return b4251_pop_parity(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_pop_parity_u(uint32_t u32X)
    __attribute__((alias("gj_u32_pop_parity_u")));
