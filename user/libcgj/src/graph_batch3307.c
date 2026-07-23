/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3307: floor integer cube root for uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_cbrt_floor_u(uint32_t x);
 *     - Largest r such that r*r*r <= x.
 *       gj_u32_cbrt_floor_u(0)==0, gj_u32_cbrt_floor_u(1)==1,
 *       gj_u32_cbrt_floor_u(8)==2, gj_u32_cbrt_floor_u(0xffffffffu)==1625.
 *   uint32_t __gj_u32_cbrt_floor_u  (alias)
 *   __libcgj_batch3307_marker = "libcgj-batch3307"
 *
 * Exclusive continuum CREATE-ONLY (3301-3310). Distinct from
 * gj_icbrt_u64 (batch578) — unique u32 floor cbrt; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3307_marker[] = "libcgj-batch3307";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Floor cube root of uX via binary search. floor(cbrt(UINT32_MAX)) is
 * 1625; mid*mid*mid always fits in uint64_t under that cap.
 */
static uint32_t
b3307_cbrt_floor(uint32_t uX)
{
	uint32_t uLo;
	uint32_t uHi;
	uint32_t uMid;
	uint64_t u64Sq;
	uint64_t u64Cu;

	if (uX < 2u) {
		return uX;
	}

	uLo = 1u;
	/* cube root of UINT32_MAX is 1625 */
	uHi = 1625u;

	while (uLo < uHi) {
		uMid = uLo + ((uHi - uLo + 1u) >> 1);
		u64Sq = (uint64_t)uMid * (uint64_t)uMid;
		u64Cu = u64Sq * (uint64_t)uMid;
		if (u64Cu <= (uint64_t)uX) {
			uLo = uMid;
		} else {
			uHi = uMid - 1u;
		}
	}
	return uLo;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_cbrt_floor_u - floor integer cube root of x.
 *
 * x: unsigned 32-bit value
 *
 * Returns the largest r with r*r*r <= x. No parent wires.
 */
uint32_t
gj_u32_cbrt_floor_u(uint32_t x)
{
	(void)NULL;
	return b3307_cbrt_floor(x);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_cbrt_floor_u(uint32_t x)
    __attribute__((alias("gj_u32_cbrt_floor_u")));
