/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2919: reverse all 16 bits of a halfword
 * (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint16_t gj_bitrev16_u(uint16_t x);
 *     - Bit 0 <-> bit 15, bit 1 <-> bit 14, ... SWAR parallel reverse.
 *   uint16_t __gj_bitrev16_u  (alias)
 *   __libcgj_batch2919_marker = "libcgj-batch2919"
 *
 * Exclusive continuum wave 2911-2920. Distinct from gj_bitrev8 /
 * gj_bitrev8_u / gj_bitrev32 / gj_bitrev64. Unique surface only; no
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2919_marker[] = "libcgj-batch2919";

/* ---- freestanding helpers ---------------------------------------------- */

/* SWAR reverse over 16 bits: 1, 2, 4, then byte swap. */
static uint16_t
b2919_rev(uint16_t u16X)
{
	uint16_t u16V;

	u16V = u16X;
	u16V = (uint16_t)(((u16V >> 1) & 0x5555u) | ((u16V & 0x5555u) << 1));
	u16V = (uint16_t)(((u16V >> 2) & 0x3333u) | ((u16V & 0x3333u) << 2));
	u16V = (uint16_t)(((u16V >> 4) & 0x0f0fu) | ((u16V & 0x0f0fu) << 4));
	u16V = (uint16_t)((u16V >> 8) | (u16V << 8));
	return u16V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitrev16_u - reverse the bit order of a 16-bit halfword.
 *
 * x: value whose bits are to be reversed
 *
 * Example: 0x0001 -> 0x8000; 0x00ff -> 0xff00; 0x0000 -> 0x0000.
 * Does not call libc.
 */
uint16_t
gj_bitrev16_u(uint16_t u16X)
{
	(void)NULL;
	return b2919_rev(u16X);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_bitrev16_u(uint16_t u16X)
    __attribute__((alias("gj_bitrev16_u")));
