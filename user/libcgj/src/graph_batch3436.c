/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3436: 32-bit adjacent nibble swap (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_nibble_swap_u(uint32_t x);
 *     - Within each byte, exchange high and low 4-bit nibbles
 *       (SWAR: (x & 0x0f0f0f0f) << 4 | (x & 0xf0f0f0f0) >> 4).
 *   uint32_t __gj_u32_nibble_swap_u  (alias)
 *   __libcgj_batch3436_marker = "libcgj-batch3436"
 *
 * CREATE-ONLY exclusive continuum wave (3431-3440). Unique
 * gj_u32_nibble_swap_u surface only; no multi-def. Distinct from
 * gj_nibble_swap (batch938, single byte) and gj_u32_nibble_reverse
 * (batch2137, full nibble order reverse). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3436_marker[] = "libcgj-batch3436";

/* ---- freestanding helpers ---------------------------------------------- */

/* Swap adjacent nibbles in every byte of a 32-bit word. */
static uint32_t
b3436_nibswap(uint32_t u32X)
{
	return ((u32X & 0x0f0f0f0fu) << 4) | ((u32X & 0xf0f0f0f0u) >> 4);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_nibble_swap_u - swap high/low nibbles within each byte.
 *
 * x: value whose per-byte nibble pairs are to be swapped
 *
 * Example: 0x12ab34cd -> 0x21ba43dc. Does not call libc. No parent wires.
 */
uint32_t
gj_u32_nibble_swap_u(uint32_t u32X)
{
	(void)NULL;
	return b3436_nibswap(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_nibble_swap_u(uint32_t u32X)
    __attribute__((alias("gj_u32_nibble_swap_u")));
