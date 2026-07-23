/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3701: ceiling integer square root for uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_sqrt_ceil_u(uint32_t x);
 *     - Smallest r such that r*r >= x (ceil of real square root).
 *       gj_u32_sqrt_ceil_u(0)==0, gj_u32_sqrt_ceil_u(1)==1,
 *       gj_u32_sqrt_ceil_u(2)==2, gj_u32_sqrt_ceil_u(4)==2,
 *       gj_u32_sqrt_ceil_u(0xffffffffu)==65536.
 *   uint32_t __gj_u32_sqrt_ceil_u  (alias)
 *   __libcgj_batch3701_marker = "libcgj-batch3701"
 *
 * Exclusive continuum CREATE-ONLY (3701-3710). Distinct from
 * gj_u32_isqrt_ceil_u (batch3305) and gj_isqrt_u64 (batch371 floor) —
 * unique sqrt_ceil_u surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3701_marker[] = "libcgj-batch3701";

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor isqrt of uX (binary search; mid*mid fits in uint64_t). */
static uint32_t
b3701_isqrt_floor(uint32_t uX)
{
	uint32_t uLo;
	uint32_t uHi;
	uint32_t uMid;
	uint64_t u64Sq;

	if (uX <= 1u) {
		return uX;
	}

	uLo = 1u;
	/* floor(sqrt(UINT32_MAX)) == 65535 */
	uHi = (uX < 0xffffu) ? uX : 0xffffu;

	while (uLo < uHi) {
		uMid = uLo + ((uHi - uLo + 1u) >> 1);
		u64Sq = (uint64_t)uMid * (uint64_t)uMid;
		if (u64Sq <= (uint64_t)uX) {
			uLo = uMid;
		} else {
			uHi = uMid - 1u;
		}
	}
	return uLo;
}

/* Ceiling isqrt: floor, then +1 if not a perfect square. */
static uint32_t
b3701_sqrt_ceil(uint32_t uX)
{
	uint32_t uR;
	uint64_t u64Sq;

	if (uX <= 1u) {
		return uX;
	}

	uR = b3701_isqrt_floor(uX);
	u64Sq = (uint64_t)uR * (uint64_t)uR;
	if (u64Sq == (uint64_t)uX) {
		return uR;
	}
	/* Not perfect: next integer. Cap at 65536 for UINT32_MAX. */
	return uR + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_sqrt_ceil_u - ceiling integer square root of x.
 *
 * x: unsigned 32-bit value
 *
 * Returns the smallest r with r*r >= x. No parent wires.
 */
uint32_t
gj_u32_sqrt_ceil_u(uint32_t x)
{
	(void)NULL;
	return b3701_sqrt_ceil(x);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_sqrt_ceil_u(uint32_t x)
    __attribute__((alias("gj_u32_sqrt_ceil_u")));
