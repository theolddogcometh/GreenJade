/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2133: freestanding uint32 full bit reverse.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_bit_reverse(uint32_t x);
 *     - Reverse bit order of a 32-bit value (bit 0 <-> bit 31, ...).
 *       SWAR parallel reverse; pure integer, no builtins or tables.
 *   uint32_t __gj_u32_bit_reverse  (alias)
 *   __libcgj_batch2133_marker = "libcgj-batch2133"
 *
 * Post-2130 bit reverse exclusive wave (2131-2140). Distinct from
 * gj_bit_reverse32 (batch418) and gj_bitrev32 (batch181) - unique
 * surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2133_marker[] = "libcgj-batch2133";

/* ---- freestanding helpers ---------------------------------------------- */

/* SWAR reverse of all 32 bits: stages 1, 2, 4, 8, then halfword swap. */
static uint32_t
b2133_bitrev(uint32_t u32X)
{
	uint32_t u32V;

	u32V = u32X;
	u32V = ((u32V >> 1) & 0x55555555u) | ((u32V & 0x55555555u) << 1);
	u32V = ((u32V >> 2) & 0x33333333u) | ((u32V & 0x33333333u) << 2);
	u32V = ((u32V >> 4) & 0x0f0f0f0fu) | ((u32V & 0x0f0f0f0fu) << 4);
	u32V = ((u32V >> 8) & 0x00ff00ffu) | ((u32V & 0x00ff00ffu) << 8);
	u32V = (u32V >> 16) | (u32V << 16);
	return u32V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_bit_reverse - reverse the bit order of a 32-bit word.
 *
 * x: value whose bits are to be reversed
 *
 * Example: 0x00000001 -> 0x80000000; 0x80000000 -> 0x00000001.
 */
uint32_t
gj_u32_bit_reverse(uint32_t u32X)
{
	(void)NULL;
	return b2133_bitrev(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_bit_reverse(uint32_t u32X)
    __attribute__((alias("gj_u32_bit_reverse")));
