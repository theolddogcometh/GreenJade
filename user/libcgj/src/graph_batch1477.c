/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1477: Q8.8 saturating divide.
 *
 * Surface (unique symbols):
 *   int32_t gj_q8_div(int32_t a, int32_t b);
 *     — (a << 8) / b via int64 intermediate; result saturates to
 *       INT32_MIN / INT32_MAX. Div-by-zero → 0.
 *       Operands are Q8.8 (1.0 == 0x0100).
 *   int32_t __gj_q8_div  (alias)
 *   __libcgj_batch1477_marker = "libcgj-batch1477"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1477_marker[] = "libcgj-batch1477";

/* ---- static helpers ---------------------------------------------------- */

static int32_t
s_q8_sat_i64(int64_t i64V)
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
 * gj_q8_div — divide two Q8.8 values with saturation.
 *
 * Numerator is shifted left by 8 before division. Division by zero
 * returns 0 (no errno; freestanding-safe). Quotient is clamped to the
 * signed 32-bit range.
 */
int32_t
gj_q8_div(int32_t i32A, int32_t i32B)
{
	int64_t i64Num;
	int64_t i64Quot;

	if (i32B == 0) {
		return 0;
	}
	i64Num = (int64_t)i32A << 8;
	i64Quot = i64Num / (int64_t)i32B;
	return s_q8_sat_i64(i64Quot);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_q8_div(int32_t i32A, int32_t i32B)
    __attribute__((alias("gj_q8_div")));
