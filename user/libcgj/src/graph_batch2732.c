/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2732: 32-bit population count (_u continuum).
 *
 * Surface (unique symbols):
 *   unsigned gj_popcount_u32_u(uint32_t x);
 *     - Number of 1-bits in a 32-bit word (0..32). SWAR scalar path.
 *   unsigned __gj_popcount_u32_u  (alias)
 *   __libcgj_batch2732_marker = "libcgj-batch2732"
 *
 * Bitops exclusive wave (2731-2740). Distinct from gj_popcount32
 * (batch139) — unique gj_popcount_u32_u surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2732_marker[] = "libcgj-batch2732";

/* ---- freestanding helpers ---------------------------------------------- */

/* Hamming weight via parallel bit sums (SWAR). */
static unsigned
b2732_pop32(uint32_t u32X)
{
	uint32_t x;

	x = u32X;
	x = x - ((x >> 1) & 0x55555555u);
	x = (x & 0x33333333u) + ((x >> 2) & 0x33333333u);
	x = (x + (x >> 4)) & 0x0f0f0f0fu;
	x = x + (x >> 8);
	x = x + (x >> 16);
	return (unsigned)(x & 0x3fu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_popcount_u32_u - count set bits in a 32-bit word.
 *
 * x: value to inspect
 *
 * Returns Hamming weight in 0..32. Does not call libc or builtins.
 */
unsigned
gj_popcount_u32_u(uint32_t u32X)
{
	(void)NULL;
	return b2732_pop32(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_popcount_u32_u(uint32_t u32X)
    __attribute__((alias("gj_popcount_u32_u")));
