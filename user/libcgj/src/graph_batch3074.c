/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3074: integer hypotenuse for uint64_t (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_hypot_u64_u(uint64_t x, uint64_t y);
 *     - Floor of sqrt(x*x + y*y). Full uint64 domain. Sum of squares
 *       is handled as a portable 128-bit magnitude (no __int128).
 *       Result saturates at UINT64_MAX when the true norm exceeds that
 *       (possible when both legs are huge: true hypot can reach
 *       ~sqrt(2)*2^64).
 *   uint64_t __gj_hypot_u64_u  (alias)
 *   __libcgj_batch3074_marker = "libcgj-batch3074"
 *
 * Exclusive continuum CREATE-ONLY (3071-3080). Unique gj_hypot_u64_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128. No float.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3074_marker[] = "libcgj-batch3074";

/* ---- freestanding helpers ---------------------------------------------- */

/* High 64 bits of a*b (portable four 32x32 products). */
static uint64_t
b3074_mul_hi(uint64_t u64A, uint64_t u64B)
{
	uint64_t al, ah, bl, bh;
	uint64_t p00, p01, p10, p11;
	uint64_t mid, hi;

	al = u64A & 0xffffffffull;
	ah = u64A >> 32;
	bl = u64B & 0xffffffffull;
	bh = u64B >> 32;

	p00 = al * bl;
	p01 = al * bh;
	p10 = ah * bl;
	p11 = ah * bh;

	mid = (p00 >> 32) + (p01 & 0xffffffffull) + (p10 & 0xffffffffull);
	hi = p11 + (p01 >> 32) + (p10 >> 32) + (mid >> 32);
	return hi;
}

/* Full 128-bit product a*b into *pHi:*pLo. */
static void
b3074_umul128(uint64_t u64A, uint64_t u64B, uint64_t *pHi, uint64_t *pLo)
{
	*pLo = u64A * u64B;
	*pHi = b3074_mul_hi(u64A, u64B);
}

/*
 * Add two 128-bit values. Returns 1 if the sum needs a 129th bit.
 * *pHi:*pLo get the low 128 bits of the sum.
 */
static uint64_t
b3074_add128(uint64_t u64AHi, uint64_t u64ALo, uint64_t u64BHi,
    uint64_t u64BLo, uint64_t *pHi, uint64_t *pLo)
{
	uint64_t u64SLo;
	uint64_t u64CarryLo;
	uint64_t u64T;
	uint64_t u64SHi;
	uint64_t u64COut;

	u64SLo = u64ALo + u64BLo;
	u64CarryLo = (u64SLo < u64ALo) ? 1ull : 0ull;

	u64T = u64AHi + u64BHi;
	u64COut = (u64T < u64AHi) ? 1ull : 0ull;
	u64SHi = u64T + u64CarryLo;
	if (u64SHi < u64T) {
		u64COut = 1ull;
	}

	*pLo = u64SLo;
	*pHi = u64SHi;
	return u64COut;
}

/* Compare a_hi:a_lo <= b_hi:b_lo. */
static int
b3074_ule128(uint64_t u64AHi, uint64_t u64ALo, uint64_t u64BHi,
    uint64_t u64BLo)
{
	if (u64AHi < u64BHi) {
		return 1;
	}
	if (u64AHi > u64BHi) {
		return 0;
	}
	return (u64ALo <= u64BLo) ? 1 : 0;
}

/* Floor isqrt of a 64-bit value. */
static uint64_t
b3074_isqrt_u64(uint64_t u64N)
{
	uint64_t u64Lo;
	uint64_t u64Hi;
	uint64_t u64Mid;
	uint64_t u64Sq;

	if (u64N <= 1ull) {
		return u64N;
	}

	u64Lo = 1ull;
	u64Hi = (u64N < 0xffffffffull) ? u64N : 0xffffffffull;

	while (u64Lo < u64Hi) {
		u64Mid = u64Lo + ((u64Hi - u64Lo + 1ull) >> 1);
		u64Sq = u64Mid * u64Mid;
		if (u64Sq <= u64N) {
			u64Lo = u64Mid;
		} else {
			u64Hi = u64Mid - 1ull;
		}
	}
	return u64Lo;
}

/*
 * Floor isqrt of a 128-bit value. If u64COut != 0 the true magnitude
 * is >= 2^128 and the root cannot fit in uint64_t -> UINT64_MAX.
 */
static uint64_t
b3074_isqrt128(uint64_t u64Hi, uint64_t u64Lo, uint64_t u64COut)
{
	uint64_t u64LoB;
	uint64_t u64HiB;
	uint64_t u64Mid;
	uint64_t u64SqHi;
	uint64_t u64SqLo;

	if (u64COut != 0ull) {
		return UINT64_MAX;
	}
	if (u64Hi == 0ull) {
		return b3074_isqrt_u64(u64Lo);
	}

	u64LoB = 0x100000000ull;
	u64HiB = UINT64_MAX;

	while (u64LoB < u64HiB) {
		u64Mid = u64LoB + ((u64HiB - u64LoB + 1ull) >> 1);
		b3074_umul128(u64Mid, u64Mid, &u64SqHi, &u64SqLo);
		if (b3074_ule128(u64SqHi, u64SqLo, u64Hi, u64Lo)) {
			u64LoB = u64Mid;
		} else {
			u64HiB = u64Mid - 1ull;
		}
	}
	return u64LoB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hypot_u64_u - floor(sqrt(x^2 + y^2)) for uint64 legs.
 *
 * x, y: non-negative legs
 *
 * Portable 128-bit sum of squares (no __int128). Saturates at
 * UINT64_MAX when the true norm does not fit in 64 bits.
 * No parent wires.
 */
uint64_t
gj_hypot_u64_u(uint64_t u64X, uint64_t u64Y)
{
	uint64_t u64XxHi, u64XxLo;
	uint64_t u64YyHi, u64YyLo;
	uint64_t u64SumHi, u64SumLo;
	uint64_t u64C;

	(void)NULL;

	if (u64X == 0ull) {
		return u64Y;
	}
	if (u64Y == 0ull) {
		return u64X;
	}

	b3074_umul128(u64X, u64X, &u64XxHi, &u64XxLo);
	b3074_umul128(u64Y, u64Y, &u64YyHi, &u64YyLo);
	u64C = b3074_add128(u64XxHi, u64XxLo, u64YyHi, u64YyLo,
	    &u64SumHi, &u64SumLo);
	return b3074_isqrt128(u64SumHi, u64SumLo, u64C);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_hypot_u64_u(uint64_t u64X, uint64_t u64Y)
    __attribute__((alias("gj_hypot_u64_u")));
