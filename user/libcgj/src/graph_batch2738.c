/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2738: 32-bit population parity (_u continuum).
 *
 * Surface (unique symbols):
 *   unsigned gj_parity_u32_u(uint32_t x);
 *     - Return 0 if x has even popcount, 1 if odd (including x == 0 -> 0).
 *       XOR-fold pure integer path; no builtins.
 *   unsigned __gj_parity_u32_u  (alias)
 *   __libcgj_batch2738_marker = "libcgj-batch2738"
 *
 * Bitops exclusive wave (2731-2740). Distinct from gj_parity_u32
 * (batch789, int return) — unique gj_parity_u32_u surface only; no
 * multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2738_marker[] = "libcgj-batch2738";

/* ---- freestanding helpers ---------------------------------------------- */

/* Successive XOR-folds collapse to the LSB. */
static unsigned
b2738_parity32(uint32_t u32X)
{
	uint32_t u32V;

	u32V = u32X;
	u32V ^= u32V >> 16;
	u32V ^= u32V >> 8;
	u32V ^= u32V >> 4;
	u32V ^= u32V >> 2;
	u32V ^= u32V >> 1;
	return (unsigned)(u32V & 1u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_parity_u32_u - parity of set bits in a 32-bit word.
 *
 * x: value to inspect
 *
 * Returns 0 for even popcount (including x == 0), 1 for odd.
 * Does not call libc.
 */
unsigned
gj_parity_u32_u(uint32_t u32X)
{
	(void)NULL;
	return b2738_parity32(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_parity_u32_u(uint32_t u32X)
    __attribute__((alias("gj_parity_u32_u")));
