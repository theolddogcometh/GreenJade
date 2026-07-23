/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3431: 64-bit population parity (unsigned, wave rename).
 *
 * Surface (unique symbols):
 *   unsigned gj_u64_parity_u2(uint64_t x);
 *     - Return 0 if x has even popcount, 1 if odd (including x == 0 -> 0).
 *       XOR-fold pure integer path; no builtins.
 *       Renamed from gj_u64_parity_u (collision with batch2289).
 *   unsigned __gj_u64_parity_u2  (alias)
 *   __libcgj_batch3431_marker = "libcgj-batch3431"
 *
 * CREATE-ONLY exclusive continuum wave (3431-3440): u64_parity_u2,
 * u32_byte_swap_u, u64_byte_swap_u, u32_word_swap_u, u64_dword_swap_u,
 * u32_nibble_swap_u, u64_nibble_swap_u, u32_bit_interleave_u,
 * u32_bit_deinterleave_u, batch_id_3440. Unique gj_u64_parity_u2 surface
 * only; no multi-def. Distinct from gj_u64_parity_u (batch2289),
 * gj_parity64 (batch419), gj_parity_u64 (batch595). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3431_marker[] = "libcgj-batch3431";

/* ---- freestanding helpers ---------------------------------------------- */

/* Successive XOR-folds collapse to the LSB parity bit. */
static unsigned
b3431_parity64(uint64_t u64X)
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
 * gj_u64_parity_u2 - parity of set bits in a 64-bit word.
 *
 * x: value to inspect
 *
 * Returns 0 for even popcount (including x == 0), 1 for odd.
 * Wave-unique rename: gj_u64_parity_u is owned by batch2289.
 * Does not call libc. No parent wires.
 */
unsigned
gj_u64_parity_u2(uint64_t u64X)
{
	(void)NULL;
	return b3431_parity64(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u64_parity_u2(uint64_t u64X)
    __attribute__((alias("gj_u64_parity_u2")));
