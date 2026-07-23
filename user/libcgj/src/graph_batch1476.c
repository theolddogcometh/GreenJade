/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1476: Q8.8 saturating multiply.
 *
 * Surface (unique symbols):
 *   int32_t gj_q8_mul(int32_t a, int32_t b);
 *     — (a * b) >> 8 via int64 intermediate; result saturates to
 *       INT32_MIN / INT32_MAX. Operands are Q8.8 (1.0 == 0x0100).
 *   int32_t __gj_q8_mul  (alias)
 *   __libcgj_batch1476_marker = "libcgj-batch1476"
 *
 * Q8.8 layout (stored in signed 32-bit for headroom):
 *   low 8 bits = fractional part (1/256 units)
 *   remaining  = integer part (two's complement)
 *   1.0 == 0x00000100
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1476_marker[] = "libcgj-batch1476";

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
 * gj_q8_mul — multiply two Q8.8 values with saturation.
 *
 * Product is computed in int64_t then arithmetically shifted right by 8
 * to restore Q8.8 scale. Out-of-range results clamp to INT32_MIN/MAX.
 */
int32_t
gj_q8_mul(int32_t i32A, int32_t i32B)
{
	int64_t i64Prod;

	i64Prod = (int64_t)i32A * (int64_t)i32B;
	return s_q8_sat_i64(i64Prod >> 8);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_q8_mul(int32_t i32A, int32_t i32B)
    __attribute__((alias("gj_q8_mul")));
