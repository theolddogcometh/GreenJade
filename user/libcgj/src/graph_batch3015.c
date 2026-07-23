/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3015: 16-bit population count (exclusive
 * popcnt_u).
 *
 * Surface (unique symbols):
 *   unsigned gj_u16_popcnt_u(uint16_t x);
 *     - Number of 1-bits in a 16-bit word (0..16). SWAR path; no
 *       compiler builtins / libgcc popcount dependency.
 *   unsigned __gj_u16_popcnt_u  (alias)
 *   __libcgj_batch3015_marker = "libcgj-batch3015"
 *
 * Exclusive continuum CREATE-ONLY wave 3011-3020. Distinct from
 * gj_u16_popcount (batch2282), gj_popcnt_u16 (batch786). Unique
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3015_marker[] = "libcgj-batch3015";

/* ---- freestanding helpers ---------------------------------------------- */

/* SWAR Hamming weight of one uint16_t. */
static unsigned
b3015_pop16(uint16_t u16X)
{
	uint32_t u;

	u = (uint32_t)u16X;
	u = u - ((u >> 1) & 0x5555u);
	u = (u & 0x3333u) + ((u >> 2) & 0x3333u);
	u = (u + (u >> 4)) & 0x0f0fu;
	u = (u + (u >> 8)) & 0x001fu;
	return (unsigned)u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u16_popcnt_u - population count of a 16-bit word.
 *
 * x: value to inspect
 *
 * Returns the number of set bits in [0, 16]. x == 0 -> 0.
 * Does not call libc.
 */
unsigned
gj_u16_popcnt_u(uint16_t u16X)
{
	(void)NULL;
	return b3015_pop16(u16X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u16_popcnt_u(uint16_t u16X)
    __attribute__((alias("gj_u16_popcnt_u")));
