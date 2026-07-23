/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch172: Q16.16 fixed-point arithmetic helpers.
 * Clean-room freestanding pure C — integer only (builds under userspace
 * -msse2; this TU does not require SSE). No third-party fixed-point
 * source was copied.
 *
 * Q16.16 layout (signed 32-bit):
 *   bits 31..16 = integer part (two's complement)
 *   bits 15..0  = fractional part (1/65536 units)
 *   1.0 == 0x00010000
 *
 * Surface (unique symbols):
 *   int32_t gj_q16_mul(int32_t a, int32_t b);
 *     — (a * b) >> 16 via int64 intermediate (no overflow of product).
 *   int32_t gj_q16_div(int32_t a, int32_t b);
 *     — (a << 16) / b via int64 intermediate. Div-by-zero → 0.
 *   int32_t gj_q16_from_int(int v);
 *     — integer → Q16.16 (v << 16). Large |v| truncates to int32_t.
 *   int     gj_q16_to_int(int32_t q);
 *     — Q16.16 → integer, truncate toward zero (discard fraction).
 *   __gj_q16_mul / __gj_q16_div / __gj_q16_from_int / __gj_q16_to_int
 *   __libcgj_batch172_marker = "libcgj-batch172"
 */

#include <stdint.h>

const char __libcgj_batch172_marker[] = "libcgj-batch172";

/* Fractional scale: 2^16. */
#define B172_Q16_ONE  ((int32_t)0x00010000)

/* ---- public: Q16.16 helpers -------------------------------------------- */

/*
 * gj_q16_mul — multiply two Q16.16 values.
 * Product is computed in int64_t then shifted right by 16 to restore scale.
 */
int32_t
gj_q16_mul(int32_t i32A, int32_t i32B)
{
	int64_t i64Prod;

	i64Prod = (int64_t)i32A * (int64_t)i32B;
	return (int32_t)(i64Prod >> 16);
}

int32_t __gj_q16_mul(int32_t i32A, int32_t i32B)
    __attribute__((alias("gj_q16_mul")));

/*
 * gj_q16_div — divide two Q16.16 values.
 * Numerator is shifted left by 16 before division. Division by zero
 * returns 0 (no errno; freestanding-safe).
 */
int32_t
gj_q16_div(int32_t i32A, int32_t i32B)
{
	int64_t i64Num;

	if (i32B == 0) {
		return 0;
	}
	i64Num = (int64_t)i32A << 16;
	return (int32_t)(i64Num / (int64_t)i32B);
}

int32_t __gj_q16_div(int32_t i32A, int32_t i32B)
    __attribute__((alias("gj_q16_div")));

/*
 * gj_q16_from_int — convert a signed integer to Q16.16.
 * Values outside the signed 16-bit integer range wrap/truncate when
 * cast back to int32_t after the shift (callers should keep |v| <= 32767
 * for a full-precision integer part).
 */
int32_t
gj_q16_from_int(int nVal)
{
	return (int32_t)((int64_t)nVal << 16);
}

int32_t __gj_q16_from_int(int nVal)
    __attribute__((alias("gj_q16_from_int")));

/*
 * gj_q16_to_int — convert Q16.16 to signed integer, truncate toward zero.
 * Uses C division (not arithmetic right shift) so negative fractions
 * round toward zero rather than toward −∞.
 */
int
gj_q16_to_int(int32_t i32Q)
{
	return (int)(i32Q / B172_Q16_ONE);
}

int __gj_q16_to_int(int32_t i32Q)
    __attribute__((alias("gj_q16_to_int")));
