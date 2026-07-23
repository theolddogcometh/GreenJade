/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1471: Q16.16 saturating multiply.
 *
 * Surface (unique symbols for this TU):
 *   int32_t gj_q16_mul_sat(int32_t a, int32_t b);
 *     — (a * b) >> 16 via int64 intermediate; result saturates to
 *       INT32_MIN / INT32_MAX (no wrap).
 *   int32_t __gj_q16_mul_sat  (alias)
 *   __libcgj_batch1471_marker = "libcgj-batch1471"
 *
 * Q16.16 layout (signed 32-bit):
 *   bits 31..16 = integer part (two's complement)
 *   bits 15..0  = fractional part (1/65536 units)
 *   1.0 == 0x00010000
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1471_marker[] = "libcgj-batch1471";

/* ---- static helpers ---------------------------------------------------- */

/*
 * Clamp a signed 64-bit value into the int32_t range (saturating).
 */
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
 * gj_q16_mul_sat — multiply two Q16.16 values with saturation.
 *
 * Product is computed in int64_t then arithmetically shifted right by 16
 * to restore Q16.16 scale. Out-of-range results clamp to INT32_MIN/MAX.
 */
int32_t
gj_q16_mul_sat(int32_t i32A, int32_t i32B)
{
	int64_t i64Prod;

	i64Prod = (int64_t)i32A * (int64_t)i32B;
	return s_q16_sat_i64(i64Prod >> 16);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_q16_mul_sat(int32_t i32A, int32_t i32B)
    __attribute__((alias("gj_q16_mul_sat")));
