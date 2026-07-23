/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch959: integer linear interpolation.
 *
 * Surface (unique symbols):
 *   int32_t gj_lerp_i32(int32_t a, int32_t b, int32_t t, int32_t t_max);
 *     — Return a + (b - a) * t / t_max using 64-bit intermediate math.
 *       t_max == 0 → return a. t clamped is not applied; caller owns range.
 *   int32_t __gj_lerp_i32  (alias)
 *   __libcgj_batch959_marker = "libcgj-batch959"

 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch959_marker[] = "libcgj-batch959";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lerp_i32 — integer lerp: a + (b - a) * t / t_max.
 *
 * Uses int64_t intermediates to reduce overflow on the multiply.
 * Truncates toward zero on the division (C99 signed division).
 * When t_max is 0, returns a (undefined ratio avoided).
 */
int32_t
gj_lerp_i32(int32_t nA, int32_t nB, int32_t nT, int32_t nTMax)
{
	int64_t n64Diff;
	int64_t n64Num;
	int64_t n64Res;

	if (nTMax == 0) {
		return nA;
	}

	n64Diff = (int64_t)nB - (int64_t)nA;
	n64Num = n64Diff * (int64_t)nT;
	n64Res = (int64_t)nA + (n64Num / (int64_t)nTMax);
	return (int32_t)n64Res;
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_lerp_i32(int32_t nA, int32_t nB, int32_t nT, int32_t nTMax)
    __attribute__((alias("gj_lerp_i32")));
