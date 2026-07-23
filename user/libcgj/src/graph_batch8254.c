/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8254: population-count integer stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bit_popcount_u_8254(uint32_t v);
 *     - Number of 1-bits in v (Hamming weight). SWAR pure integer path.
 *   uint32_t __gj_bit_popcount_u_8254  (alias)
 *   __libcgj_batch8254_marker = "libcgj-batch8254"
 *
 * Exclusive continuum CREATE-ONLY (8251-8260: bitops / ffs-style integer
 * stubs — ffs, clz, ctz, popcount, parity, isolate_lsb, clear_lsb, mask,
 * test, batch_id_8260). Unique gj_bit_popcount_u_8254 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8254_marker[] = "libcgj-batch8254";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b8254_popcount — SWAR population count of a 32-bit word.
 */
static uint32_t
b8254_popcount(uint32_t u32V)
{
	uint32_t u32X;

	u32X = u32V;
	u32X = u32X - ((u32X >> 1) & 0x55555555u);
	u32X = (u32X & 0x33333333u) + ((u32X >> 2) & 0x33333333u);
	u32X = (u32X + (u32X >> 4)) & 0x0f0f0f0fu;
	u32X = u32X + (u32X >> 8);
	u32X = u32X + (u32X >> 16);
	return u32X & 0x3fu;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bit_popcount_u_8254 - count set bits in a 32-bit word.
 *
 * v: value to inspect
 *
 * Returns the Hamming weight of v (0..32). Soft popcount catalog
 * surface; pure integer SWAR, no builtins. No parent wires.
 */
uint32_t
gj_bit_popcount_u_8254(uint32_t u32V)
{
	(void)NULL;
	return b8254_popcount(u32V);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bit_popcount_u_8254(uint32_t u32V)
    __attribute__((alias("gj_bit_popcount_u_8254")));
