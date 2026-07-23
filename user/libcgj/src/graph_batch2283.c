/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2283: 32-bit population count (unsigned).
 *
 * Surface (unique symbols):
 *   unsigned gj_u32_popcount_u(uint32_t x);
 *     - Number of 1-bits in a 32-bit word (0..32). SWAR path; no
 *       compiler builtins / libgcc popcount dependency.
 *   unsigned __gj_u32_popcount_u  (alias)
 *   __libcgj_batch2283_marker = "libcgj-batch2283"
 *
 * Post-2280 bitops exclusive wave (2281-2290). Distinct from
 * gj_popcount32 (batch139) - unique gj_u32_popcount_u surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2283_marker[] = "libcgj-batch2283";

/* ---- freestanding helpers ---------------------------------------------- */

/* SWAR Hamming weight of one uint32_t (Hacker's Delight style). */
static unsigned
b2283_pop32(uint32_t u32X)
{
	u32X = u32X - ((u32X >> 1) & 0x55555555u);
	u32X = (u32X & 0x33333333u) + ((u32X >> 2) & 0x33333333u);
	u32X = (u32X + (u32X >> 4)) & 0x0f0f0f0fu;
	u32X = u32X + (u32X >> 8);
	u32X = u32X + (u32X >> 16);
	return (unsigned)(u32X & 0x3fu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_popcount_u - population count of a 32-bit word.
 *
 * x: value to inspect
 *
 * Returns the number of set bits in [0, 32). x == 0 -> 0.
 * Does not call libc.
 */
unsigned
gj_u32_popcount_u(uint32_t u32X)
{
	(void)NULL;
	return b2283_pop32(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u32_popcount_u(uint32_t u32X)
    __attribute__((alias("gj_u32_popcount_u")));
