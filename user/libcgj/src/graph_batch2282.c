/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2282: 16-bit population count (unsigned).
 *
 * Surface (unique symbols):
 *   unsigned gj_u16_popcount(uint16_t x);
 *     - Number of 1-bits in a 16-bit word (0..16). SWAR path; no
 *       compiler builtins / libgcc popcount dependency.
 *   unsigned __gj_u16_popcount  (alias)
 *   __libcgj_batch2282_marker = "libcgj-batch2282"
 *
 * Post-2280 bitops exclusive wave (2281-2290). Distinct from
 * gj_popcount32 / gj_u8_popcount - unique u16 surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2282_marker[] = "libcgj-batch2282";

/* ---- freestanding helpers ---------------------------------------------- */

/* SWAR Hamming weight of one uint16_t (Hacker's Delight style). */
static unsigned
b2282_pop16(uint16_t u16X)
{
	uint32_t u;

	u = (uint32_t)u16X;
	u = u - ((u >> 1) & 0x5555u);
	u = (u & 0x3333u) + ((u >> 2) & 0x3333u);
	u = (u + (u >> 4)) & 0x0f0fu;
	u = u + (u >> 8);
	return (unsigned)(u & 0x1fu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u16_popcount - population count of a 16-bit word.
 *
 * x: value to inspect
 *
 * Returns the number of set bits in [0, 16). x == 0 -> 0.
 * Does not call libc.
 */
unsigned
gj_u16_popcount(uint16_t u16X)
{
	(void)NULL;
	return b2282_pop16(u16X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u16_popcount(uint16_t u16X)
    __attribute__((alias("gj_u16_popcount")));
