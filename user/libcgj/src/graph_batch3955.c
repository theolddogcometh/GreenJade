/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3955: freestanding uint32 nibble reverse (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_nibble_reverse_u(uint32_t x);
 *     - Reverse the order of the eight 4-bit nibbles in a 32-bit word
 *       (nibble 0 <-> nibble 7, ...). Pure integer SWAR; no builtins.
 *   uint32_t __gj_u32_nibble_reverse_u  (alias)
 *   __libcgj_batch3955_marker = "libcgj-batch3955"
 *
 * CREATE-ONLY exclusive continuum wave (3951-3960). Unique
 * gj_u32_nibble_reverse_u surface only; no multi-def. Distinct from
 * gj_u32_nibble_reverse (batch2137) and gj_u32_nibble_swap_u
 * (batch3436, adjacent nibble swap only). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3955_marker[] = "libcgj-batch3955";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Reverse eight 4-bit lanes: swap adjacent nibbles, then adjacent bytes,
 * then 16-bit halves.
 */
static uint32_t
b3955_nibrev(uint32_t u32X)
{
	uint32_t u32V;

	u32V = u32X;
	u32V = ((u32V & 0x0f0f0f0fu) << 4) | ((u32V & 0xf0f0f0f0u) >> 4);
	u32V = ((u32V & 0x00ff00ffu) << 8) | ((u32V & 0xff00ff00u) >> 8);
	u32V = (u32V << 16) | (u32V >> 16);
	return u32V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_nibble_reverse_u - reverse the eight nibbles of a 32-bit word.
 *
 * x: value whose nibble order is to be reversed
 *
 * Example: 0x12345678 -> 0x87654321; 0x0f0e0d0c -> 0xc0d0e0f0.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_u32_nibble_reverse_u(uint32_t u32X)
{
	(void)NULL;
	return b3955_nibrev(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_nibble_reverse_u(uint32_t u32X)
    __attribute__((alias("gj_u32_nibble_reverse_u")));
