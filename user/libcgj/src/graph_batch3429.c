/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3429: 32-bit population parity (unsigned).
 *
 * Surface (unique symbols):
 *   unsigned gj_u32_parity_u(uint32_t x);
 *     - Return 0 if x has even popcount, 1 if odd (including x == 0 -> 0).
 *       XOR-fold pure integer path; no builtins.
 *   unsigned __gj_u32_parity_u  (alias)
 *   __libcgj_batch3429_marker = "libcgj-batch3429"
 *
 * Bitops exclusive CREATE-ONLY wave (3421-3430). Distinct from
 * gj_parity_u32 (batch789, int return), gj_u64_parity_u (batch2289),
 * gj_parity_u32_u (batch2738) — unique gj_u32_parity_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3429_marker[] = "libcgj-batch3429";

/* ---- freestanding helpers ---------------------------------------------- */

/* Successive XOR-folds collapse to the LSB parity bit. */
static unsigned
b3429_parity32(uint32_t u32X)
{
	uint32_t v;

	v = u32X;
	v ^= v >> 16;
	v ^= v >> 8;
	v ^= v >> 4;
	v ^= v >> 2;
	v ^= v >> 1;
	return (unsigned)(v & 1u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_parity_u - parity of set bits in a 32-bit word.
 *
 * x: value to inspect
 *
 * Returns 0 for even popcount (including x == 0), 1 for odd.
 * Does not call libc. No parent wires.
 */
unsigned
gj_u32_parity_u(uint32_t u32X)
{
	(void)NULL;
	return b3429_parity32(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u32_parity_u(uint32_t u32X)
    __attribute__((alias("gj_u32_parity_u")));
