/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2921: freestanding 32-bit bit reverse (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bitrev32_u(uint32_t x);
 *     - Reverse all 32 bits of x (LSB becomes MSB and vice versa).
 *   uint32_t __gj_bitrev32_u  (alias)
 *   __libcgj_batch2921_marker = "libcgj-batch2921"
 *
 * Exclusive continuum CREATE-ONLY (2921-2930). Distinct from
 * gj_bitrev32 (batch181) — unique gj_bitrev32_u surface only; no
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2921_marker[] = "libcgj-batch2921";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b2921_rev32 - SWAR pairwise bit-group swaps (1, 2, 4, 8, 16).
 */
static uint32_t
b2921_rev32(uint32_t u32X)
{
	u32X = ((u32X >> 1) & 0x55555555u) | ((u32X & 0x55555555u) << 1);
	u32X = ((u32X >> 2) & 0x33333333u) | ((u32X & 0x33333333u) << 2);
	u32X = ((u32X >> 4) & 0x0f0f0f0fu) | ((u32X & 0x0f0f0f0fu) << 4);
	u32X = ((u32X >> 8) & 0x00ff00ffu) | ((u32X & 0x00ff00ffu) << 8);
	u32X = (u32X >> 16) | (u32X << 16);
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitrev32_u - reverse the bit order of a 32-bit word.
 * Example: 0x00000001u → 0x80000000u; 0x12345678u → 0x1e6a2c48u.
 */
uint32_t
gj_bitrev32_u(uint32_t u32X)
{
	(void)NULL;
	return b2921_rev32(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bitrev32_u(uint32_t u32X)
    __attribute__((alias("gj_bitrev32_u")));
