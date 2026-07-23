/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1472: Q16.16 saturating divide.
 *
 * Surface (unique symbols for this TU):
 *   int32_t gj_q16_div_sat(int32_t a, int32_t b);
 *     — (a << 16) / b via int64 intermediate; result saturates to
 *       INT32_MIN / INT32_MAX. Div-by-zero → 0.
 *   int32_t __gj_q16_div_sat  (alias)
 *   __libcgj_batch1472_marker = "libcgj-batch1472"
 *
 * Q16.16 layout (signed 32-bit): 1.0 == 0x00010000.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1472_marker[] = "libcgj-batch1472";

/* ---- static helpers ---------------------------------------------------- */

static int32_t
s_q16_sat_i64(int64_t i64V)
{
	if (i64V > (int64_t)INT32_MAX) {
		return INT32_MAX;
	}
	if (i64V < (int64_t)INT32_MIN) {
		return INT32_MIN;
	}
	return (int32_t)i64V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_q16_div_sat — divide two Q16.16 values with saturation.
 *
 * Numerator is shifted left by 16 before division. Division by zero
 * returns 0 (no errno; freestanding-safe). Quotient is clamped to the
 * signed 32-bit range.
 */
int32_t
gj_q16_div_sat(int32_t i32A, int32_t i32B)
{
	int64_t i64Num;
	int64_t i64Quot;

	if (i32B == 0) {
		return 0;
	}
	i64Num = (int64_t)i32A << 16;
	i64Quot = i64Num / (int64_t)i32B;
	return s_q16_sat_i64(i64Quot);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_q16_div_sat(int32_t i32A, int32_t i32B)
    __attribute__((alias("gj_q16_div_sat")));
