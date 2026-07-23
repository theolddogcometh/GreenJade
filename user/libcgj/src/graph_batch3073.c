/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3073: integer hypotenuse for uint32_t (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_hypot_u32_u(uint32_t x, uint32_t y);
 *     - Floor of sqrt(x*x + y*y). Full uint32 domain; the sum of
 *       squares may need 65 bits and the result may exceed UINT32_MAX
 *       (e.g. hypot(UINT32_MAX, UINT32_MAX)), so the return is uint64_t.
 *   uint64_t __gj_hypot_u32_u  (alias)
 *   __libcgj_batch3073_marker = "libcgj-batch3073"
 *
 * Exclusive continuum CREATE-ONLY (3071-3080). Unique gj_hypot_u32_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128. No float.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3073_marker[] = "libcgj-batch3073";

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor isqrt of a 64-bit value (binary search; root fits in 32 bits). */
static uint64_t
b3073_isqrt_u64(uint64_t u64N)
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
 * mid*mid for mid < 2^33 into 128-bit (hi may be 0..3).
 * mid = m1*2^32 + m0 with m1 in {0,1} for our search range, but allow
 * m1 small generally via 32-bit split.
 */
static void
b3073_sq33(uint64_t u64Mid, uint64_t *pHi, uint64_t *pLo)
{
	uint64_t m0;
	uint64_t m1;
	uint64_t p00;
	uint64_t p01;
	uint64_t p10;
	uint64_t p11;
	uint64_t mid;
	uint64_t lo;
	uint64_t hi;

	m0 = u64Mid & 0xffffffffull;
	m1 = u64Mid >> 32;

	p00 = m0 * m0;
	p01 = m0 * m1;
	p10 = m1 * m0; /* == p01 */
	p11 = m1 * m1;

	/* Same fold as portable mul_hi, full product. */
	mid = (p00 >> 32) + (p01 & 0xffffffffull) + (p10 & 0xffffffffull);
	lo = (p00 & 0xffffffffull) | (mid << 32);
	hi = p11 + (p01 >> 32) + (p10 >> 32) + (mid >> 32);

	*pLo = lo;
	*pHi = hi;
}

/* Compare a_hi:a_lo <= b_hi:b_lo. */
static int
b3073_ule128(uint64_t u64AHi, uint64_t u64ALo, uint64_t u64BHi,
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

/*
 * Floor isqrt of hi:lo where the magnitude is at most ~2^65
 * (x^2+y^2 for uint32 x,y). Result < 2^33.
 */
static uint64_t
b3073_isqrt_65(uint64_t u64Hi, uint64_t u64Lo)
{
	uint64_t u64LoB;
	uint64_t u64HiB;
	uint64_t u64Mid;
	uint64_t u64SqHi;
	uint64_t u64SqLo;

	if (u64Hi == 0ull) {
		return b3073_isqrt_u64(u64Lo);
	}

	/* hi == 1: n in [2^64, 2*2^64). Root in [2^32, ceil(sqrt(2)*2^32)]. */
	u64LoB = 0x100000000ull;
	u64HiB = 0x16a09e668ull;

	while (u64LoB < u64HiB) {
		u64Mid = u64LoB + ((u64HiB - u64LoB + 1ull) >> 1);
		b3073_sq33(u64Mid, &u64SqHi, &u64SqLo);
		if (b3073_ule128(u64SqHi, u64SqLo, u64Hi, u64Lo)) {
			u64LoB = u64Mid;
		} else {
			u64HiB = u64Mid - 1ull;
		}
	}
	return u64LoB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hypot_u32_u - floor(sqrt(x^2 + y^2)) for uint32 legs.
 *
 * x, y: non-negative legs
 *
 * Returns the Euclidean norm floored to an integer (uint64_t).
 * No parent wires.
 */
uint64_t
gj_hypot_u32_u(uint32_t u32X, uint32_t u32Y)
{
	uint64_t u64Xx;
	uint64_t u64Yy;
	uint64_t u64Sum;
	uint64_t u64Hi;

	(void)NULL;

	if (u32X == 0u) {
		return (uint64_t)u32Y;
	}
	if (u32Y == 0u) {
		return (uint64_t)u32X;
	}

	u64Xx = (uint64_t)u32X * (uint64_t)u32X;
	u64Yy = (uint64_t)u32Y * (uint64_t)u32Y;
	u64Sum = u64Xx + u64Yy;
	u64Hi = (u64Sum < u64Xx) ? 1ull : 0ull;
	return b3073_isqrt_65(u64Hi, u64Sum);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_hypot_u32_u(uint32_t u32X, uint32_t u32Y)
    __attribute__((alias("gj_hypot_u32_u")));
