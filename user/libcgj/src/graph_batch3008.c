/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3008: high 64 bits of uint64×uint64 product
 * (portable, no __int128) (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_mul_hi_portable_u(uint64_t a, uint64_t b);
 *     - Return floor((a * b) / 2^64), i.e. the high half of the
 *       full 128-bit unsigned product. Implemented with four 32×32
 *       multiplies and carry fold (no __int128).
 *   uint64_t __gj_u64_mul_hi_portable_u  (alias)
 *   __libcgj_batch3008_marker = "libcgj-batch3008"
 *
 * Exclusive continuum CREATE-ONLY (3001-3010). Distinct from
 * gj_mul_hi_u64 (batch568) — unique gj_u64_mul_hi_portable_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3008_marker[] = "libcgj-batch3008";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * High 64 bits of a*b via 32-bit half multiplies.
 * a = ah:al, b = bh:bl
 *   p00 = al*bl, p01 = al*bh, p10 = ah*bl, p11 = ah*bh
 *   mid = (p00>>32) + (p01 & 0xffffffff) + (p10 & 0xffffffff)
 *   hi  = p11 + (p01>>32) + (p10>>32) + (mid>>32)
 */
static uint64_t
b3008_mul_hi(uint64_t u64A, uint64_t u64B)
{
	uint64_t al, ah, bl, bh;
	uint64_t p00, p01, p10, p11;
	uint64_t mid, hi;

	al = u64A & 0xffffffffu;
	ah = u64A >> 32;
	bl = u64B & 0xffffffffu;
	bh = u64B >> 32;

	p00 = al * bl;
	p01 = al * bh;
	p10 = ah * bl;
	p11 = ah * bh;

	mid = (p00 >> 32) + (p01 & 0xffffffffu) + (p10 & 0xffffffffu);
	hi = p11 + (p01 >> 32) + (p10 >> 32) + (mid >> 32);

	return hi;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_mul_hi_portable_u - high 64 bits of a 128-bit unsigned product.
 *
 * a, b: factors
 * Portable four-multiply fold; no __int128. No parent wires.
 */
uint64_t
gj_u64_mul_hi_portable_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b3008_mul_hi(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_mul_hi_portable_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_mul_hi_portable_u")));
