/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4729: integer hypotenuse for uint32 legs.
 *
 * Surface (unique symbols):
 *   uint64_t gj_ihypot_u32_u(uint32_t a, uint32_t b);
 *     - Floor of sqrt(a*a + b*b). Full uint32 domain; sum of squares may
 *       need 65 bits and the result may exceed UINT32_MAX, so the return
 *       is uint64_t. Overflow-safe (no __int128).
 *   uint64_t __gj_ihypot_u32_u  (alias)
 *   __libcgj_batch4729_marker = "libcgj-batch4729"
 *
 * Exclusive continuum CREATE-ONLY (4721-4730). Distinct from
 * gj_hypot_u32_u (batch3073) — unique gj_ihypot_u32_u surface only; no
 * multi-def. Does not call public isqrt symbols (no parent wires).
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4729_marker[] = "libcgj-batch4729";

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor isqrt of a value that fits in 64 bits (root fits in 32 bits). */
static uint64_t
b4729_isqrt_u64(uint64_t u64N)
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
 * Square a value that may be up to ~ceil(sqrt(2)*2^32) (< 2^33) into
 * a 128-bit product represented as hi:lo, without __int128.
 */
static void
b4729_sq(uint64_t u64Mid, uint64_t *pHi, uint64_t *pLo)
{
	uint64_t u64M0;
	uint64_t u64M1;
	uint64_t u64P00;
	uint64_t u64P01;
	uint64_t u64P10;
	uint64_t u64P11;
	uint64_t u64MidAcc;
	uint64_t u64Lo;
	uint64_t u64Hi;

	u64M0 = u64Mid & 0xffffffffull;
	u64M1 = u64Mid >> 32;

	u64P00 = u64M0 * u64M0;
	u64P01 = u64M0 * u64M1;
	u64P10 = u64M1 * u64M0;
	u64P11 = u64M1 * u64M1;

	u64MidAcc = (u64P00 >> 32) + (u64P01 & 0xffffffffull)
	    + (u64P10 & 0xffffffffull);
	u64Lo = (u64P00 & 0xffffffffull) | (u64MidAcc << 32);
	u64Hi = u64P11 + (u64P01 >> 32) + (u64P10 >> 32) + (u64MidAcc >> 32);

	*pLo = u64Lo;
	*pHi = u64Hi;
}

/* Compare a_hi:a_lo <= b_hi:b_lo. */
static int
b4729_ule128(uint64_t u64AHi, uint64_t u64ALo, uint64_t u64BHi,
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
 * Floor isqrt of a 65-bit magnitude hi:lo (hi is 0 or 1 for uint32 legs).
 * Result < 2^33.
 */
static uint64_t
b4729_isqrt_65(uint64_t u64Hi, uint64_t u64Lo)
{
	uint64_t u64LoB;
	uint64_t u64HiB;
	uint64_t u64Mid;
	uint64_t u64SqHi;
	uint64_t u64SqLo;

	if (u64Hi == 0ull) {
		return b4729_isqrt_u64(u64Lo);
	}

	/* hi == 1: n in [2^64, 2*2^64). Root in [2^32, ceil(sqrt(2)*2^32)]. */
	u64LoB = 0x100000000ull;
	u64HiB = 0x16a09e668ull; /* ceil(sqrt(2)*2^32) bound */

	while (u64LoB < u64HiB) {
		u64Mid = u64LoB + ((u64HiB - u64LoB + 1ull) >> 1);
		b4729_sq(u64Mid, &u64SqHi, &u64SqLo);
		if (b4729_ule128(u64SqHi, u64SqLo, u64Hi, u64Lo)) {
			u64LoB = u64Mid;
		} else {
			u64HiB = u64Mid - 1ull;
		}
	}
	return u64LoB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ihypot_u32_u - floor(sqrt(a^2 + b^2)) for uint32 legs.
 *
 * a, b: non-negative legs
 *
 * Returns the Euclidean norm floored to an integer (uint64_t). Handles
 * a*a + b*b overflow of 64 bits (carry into bit 64). Distinct from
 * gj_hypot_u32_u (batch3073). No parent wires. No __int128.
 */
uint64_t
gj_ihypot_u32_u(uint32_t u32A, uint32_t u32B)
{
	uint64_t u64Aa;
	uint64_t u64Bb;
	uint64_t u64Sum;
	uint64_t u64Hi;

	(void)NULL;

	if (u32A == 0u) {
		return (uint64_t)u32B;
	}
	if (u32B == 0u) {
		return (uint64_t)u32A;
	}

	u64Aa = (uint64_t)u32A * (uint64_t)u32A;
	u64Bb = (uint64_t)u32B * (uint64_t)u32B;
	u64Sum = u64Aa + u64Bb;
	u64Hi = (u64Sum < u64Aa) ? 1ull : 0ull;
	return b4729_isqrt_65(u64Hi, u64Sum);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_ihypot_u32_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_ihypot_u32_u")));
