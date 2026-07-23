/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2281: 8-bit population count (unsigned).
 *
 * Surface (unique symbols):
 *   unsigned gj_u8_popcount(uint8_t x);
 *     - Number of 1-bits in an 8-bit word (0..8). SWAR path; no
 *       compiler builtins / libgcc popcount dependency.
 *   unsigned __gj_u8_popcount  (alias)
 *   __libcgj_batch2281_marker = "libcgj-batch2281"
 *
 * Post-2280 bitops exclusive wave (2281-2290). Distinct from
 * gj_popcount32 / gj_popcount64 (batch139), gj_bitset64_popcount
 * (batch1631) - unique u8 surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2281_marker[] = "libcgj-batch2281";

/* ---- freestanding helpers ---------------------------------------------- */

/* SWAR Hamming weight of one uint8_t (Hacker's Delight style). */
static unsigned
b2281_pop8(uint8_t u8X)
{
	uint32_t u;

	u = (uint32_t)u8X;
	u = u - ((u >> 1) & 0x55u);
	u = (u & 0x33u) + ((u >> 2) & 0x33u);
	u = (u + (u >> 4)) & 0x0fu;
	return (unsigned)u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u8_popcount - population count of an 8-bit word.
 *
 * x: value to inspect
 *
 * Returns the number of set bits in [0, 8). x == 0 -> 0.
 * Does not call libc.
 */
unsigned
gj_u8_popcount(uint8_t u8X)
{
	(void)NULL;
	return b2281_pop8(u8X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u8_popcount(uint8_t u8X)
    __attribute__((alias("gj_u8_popcount")));
