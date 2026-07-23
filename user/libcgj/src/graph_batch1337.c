/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1337: bit interleave of two 16-bit halves into a Morton/Z-order u32.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_interleave(uint16_t x, uint16_t y);
 *     — Interleave bits of x (even positions) and y (odd positions):
 *       result bit 2*i   = x bit i; result bit 2*i+1 = y bit i.
 *       Pure integer, no builtins.
 *   uint32_t __gj_u32_interleave  (alias)
 *   __libcgj_batch1337_marker = "libcgj-batch1337"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1337_marker[] = "libcgj-batch1337";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Spread the low 16 bits of u into the even bit positions of a u32
 * (bit i → bit 2*i). Classic binary magic (part1by1).
 */
static uint32_t
b1337_part1by1(uint32_t u)
{
	u &= 0x0000ffffu;
	u = (u | (u << 8)) & 0x00ff00ffu;
	u = (u | (u << 4)) & 0x0f0f0f0fu;
	u = (u | (u << 2)) & 0x33333333u;
	u = (u | (u << 1)) & 0x55555555u;
	return u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_interleave — Morton-code style interleave of x (even) and y (odd).
 */
uint32_t
gj_u32_interleave(uint16_t u16X, uint16_t u16Y)
{
	return b1337_part1by1((uint32_t)u16X) |
	    (b1337_part1by1((uint32_t)u16Y) << 1);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_interleave(uint16_t u16X, uint16_t u16Y)
    __attribute__((alias("gj_u32_interleave")));
