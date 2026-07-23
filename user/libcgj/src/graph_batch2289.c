/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2289: 64-bit population parity (unsigned).
 *
 * Surface (unique symbols):
 *   unsigned gj_u64_parity_u(uint64_t x);
 *     - Return 0 if x has even popcount, 1 if odd (including x == 0 -> 0).
 *       XOR-fold pure integer path; no builtins.
 *   unsigned __gj_u64_parity_u  (alias)
 *   __libcgj_batch2289_marker = "libcgj-batch2289"
 *
 * Post-2280 bitops exclusive wave (2281-2290). Distinct from
 * gj_parity64 (batch419), gj_parity_u64 (batch595), gj_parity_u32
 * (batch789), gj_bitset64_parity (batch1632) - unique gj_u64_parity_u
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2289_marker[] = "libcgj-batch2289";

/* ---- freestanding helpers ---------------------------------------------- */

/* Successive XOR-folds collapse to the LSB parity bit. */
static unsigned
b2289_parity64(uint64_t u64X)
{
	uint64_t v;

	v = u64X;
	v ^= v >> 32;
	v ^= v >> 16;
	v ^= v >> 8;
	v ^= v >> 4;
	v ^= v >> 2;
	v ^= v >> 1;
	return (unsigned)(v & 1ull);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_parity_u - parity of set bits in a 64-bit word.
 *
 * x: value to inspect
 *
 * Returns 0 for even popcount (including x == 0), 1 for odd.
 * Does not call libc.
 */
unsigned
gj_u64_parity_u(uint64_t u64X)
{
	(void)NULL;
	return b2289_parity64(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u64_parity_u(uint64_t u64X)
    __attribute__((alias("gj_u64_parity_u")));
