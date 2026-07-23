/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8255: bit-parity integer stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bit_parity_u_8255(uint32_t v);
 *     - Return 1 if v has odd number of 1-bits, else 0 (parity bit).
 *   uint32_t __gj_bit_parity_u_8255  (alias)
 *   __libcgj_batch8255_marker = "libcgj-batch8255"
 *
 * Exclusive continuum CREATE-ONLY (8251-8260: bitops / ffs-style integer
 * stubs — ffs, clz, ctz, popcount, parity, isolate_lsb, clear_lsb, mask,
 * test, batch_id_8260). Unique gj_bit_parity_u_8255 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8255_marker[] = "libcgj-batch8255";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b8255_parity — fold XOR tree to single parity bit (odd = 1).
 */
static uint32_t
b8255_parity(uint32_t u32V)
{
	uint32_t u32X;

	u32X = u32V;
	u32X ^= u32X >> 16;
	u32X ^= u32X >> 8;
	u32X ^= u32X >> 4;
	u32X ^= u32X >> 2;
	u32X ^= u32X >> 1;
	return u32X & 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bit_parity_u_8255 - parity of set bits in a 32-bit word.
 *
 * v: value to inspect
 *
 * Returns 1 when the Hamming weight is odd, else 0. Soft parity catalog
 * surface; pure integer, no builtins. No parent wires.
 */
uint32_t
gj_bit_parity_u_8255(uint32_t u32V)
{
	(void)NULL;
	return b8255_parity(u32V);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bit_parity_u_8255(uint32_t u32V)
    __attribute__((alias("gj_bit_parity_u_8255")));
