/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch568: high 64 bits of uint64×uint64 product.
 *
 * Surface (unique symbols):
 *   uint64_t gj_mul_hi_u64(uint64_t a, uint64_t b);
 *     — Return floor((a * b) / 2^64), i.e. the high half of the
 *       full 128-bit unsigned product. Implemented with four 32×32
 *       multiplies and carry fold (no __int128).
 *   uint64_t __gj_mul_hi_u64  (alias)
 *   __libcgj_batch568_marker = "libcgj-batch568"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins, no __int128. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch568_marker[] = "libcgj-batch568";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mul_hi_u64 — high 64 bits of a 128-bit unsigned product.
 *
 * a, b: factors
 *
 * Split each operand into 32-bit halves (a = ah:al, b = bh:bl) and form:
 *   p00 = al * bl,  p01 = al * bh,  p10 = ah * bl,  p11 = ah * bh
 * Fold carries from the middle cross terms into the high word:
 *   mid = (p00 >> 32) + (p01 & 0xffffffff) + (p10 & 0xffffffff)
 *   hi  = p11 + (p01 >> 32) + (p10 >> 32) + (mid >> 32)
 * Returns hi. All steps stay within uint64_t.
 */
uint64_t
gj_mul_hi_u64(uint64_t a, uint64_t b)
{
	uint64_t al, ah, bl, bh;
	uint64_t p00, p01, p10, p11;
	uint64_t mid, hi;

	al = a & 0xffffffffu;
	ah = a >> 32;
	bl = b & 0xffffffffu;
	bh = b >> 32;

	p00 = al * bl;
	p01 = al * bh;
	p10 = ah * bl;
	p11 = ah * bh;

	/* Cross/middle column; mid >> 32 is the carry into the high half. */
	mid = (p00 >> 32) + (p01 & 0xffffffffu) + (p10 & 0xffffffffu);
	hi = p11 + (p01 >> 32) + (p10 >> 32) + (mid >> 32);

	return hi;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_mul_hi_u64(uint64_t a, uint64_t b)
    __attribute__((alias("gj_mul_hi_u64")));
