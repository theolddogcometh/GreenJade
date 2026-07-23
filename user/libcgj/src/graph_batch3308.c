/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3308: floor integer cube root for uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_cbrt_floor_u(uint64_t x);
 *     - Largest r such that r*r*r <= x (overflow-safe multiply).
 *       gj_u64_cbrt_floor_u(0)==0, gj_u64_cbrt_floor_u(1)==1,
 *       gj_u64_cbrt_floor_u(8)==2,
 *       gj_u64_cbrt_floor_u(UINT64_MAX)==2642245.
 *   uint64_t __gj_u64_cbrt_floor_u  (alias)
 *   __libcgj_batch3308_marker = "libcgj-batch3308"
 *
 * Exclusive continuum CREATE-ONLY (3301-3310). Distinct from
 * gj_icbrt_u64 (batch578) — unique cbrt_floor_u surface; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3308_marker[] = "libcgj-batch3308";

/* ---- freestanding helpers ---------------------------------------------- */

/* Multiply with overflow detection. Returns 0 and sets *pOk = 0 on overflow. */
static int
b3308_mul_ok(uint64_t u64A, uint64_t u64B, uint64_t *pu64Out)
{
	if (u64A != 0ULL && u64B > (UINT64_MAX / u64A)) {
		return 0;
	}
	*pu64Out = u64A * u64B;
	return 1;
}

/*
 * Floor cube root of u64X via binary search. floor(cbrt(UINT64_MAX))
 * is 2642245; multiplies are checked for overflow.
 */
static uint64_t
b3308_cbrt_floor(uint64_t u64X)
{
	uint64_t u64Lo;
	uint64_t u64Hi;
	uint64_t u64Mid;
	uint64_t u64Sq;
	uint64_t u64Cu;

	if (u64X < 2ULL) {
		return u64X;
	}

	u64Lo = 1ULL;
	/* cube root of UINT64_MAX is 2642245 */
	u64Hi = 2642245ULL;

	while (u64Lo < u64Hi) {
		u64Mid = u64Lo + ((u64Hi - u64Lo + 1ULL) >> 1);
		if (!b3308_mul_ok(u64Mid, u64Mid, &u64Sq)) {
			u64Hi = u64Mid - 1ULL;
			continue;
		}
		if (!b3308_mul_ok(u64Sq, u64Mid, &u64Cu)) {
			u64Hi = u64Mid - 1ULL;
			continue;
		}
		if (u64Cu <= u64X) {
			u64Lo = u64Mid;
		} else {
			u64Hi = u64Mid - 1ULL;
		}
	}
	return u64Lo;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_cbrt_floor_u - floor integer cube root of x.
 *
 * x: unsigned 64-bit value
 *
 * Returns the largest r with r*r*r <= x. No parent wires.
 */
uint64_t
gj_u64_cbrt_floor_u(uint64_t x)
{
	(void)NULL;
	return b3308_cbrt_floor(x);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_cbrt_floor_u(uint64_t x)
    __attribute__((alias("gj_u64_cbrt_floor_u")));
