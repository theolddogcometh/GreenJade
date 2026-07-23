/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1475: Q16.16 → integer, round half away from 0.
 *
 * Surface (unique symbols):
 *   int gj_q16_to_int_round(int32_t q);
 *     — Q16.16 → signed integer, round half away from zero. Saturates
 *       the intermediate add so near-INT32_MAX does not wrap.
 *   int __gj_q16_to_int_round  (alias)
 *   __libcgj_batch1475_marker = "libcgj-batch1475"
 *
 * Q16.16 layout (signed 32-bit): 1.0 == 0x00010000.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1475_marker[] = "libcgj-batch1475";

/* Half unit in Q16.16 (0.5). */
#define B1475_Q16_HALF  ((int32_t)0x00008000)

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
 * gj_q16_to_int_round — convert Q16.16 to signed integer, round half away
 * from zero.
 *
 * Positive: add +0.5 then arithmetic >> 16.
 * Negative: subtract 0.5 then arithmetic >> 16.
 * Intermediate bias is applied in int64_t and saturated before the shift
 * so values near the int32 extremes cannot wrap.
 */
int
gj_q16_to_int_round(int32_t i32Q)
{
	int64_t i64Adj;

	if (i32Q >= 0) {
		i64Adj = (int64_t)i32Q + (int64_t)B1475_Q16_HALF;
	} else {
		i64Adj = (int64_t)i32Q - (int64_t)B1475_Q16_HALF;
	}
	return (int)(s_q16_sat_i64(i64Adj) >> 16);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_q16_to_int_round(int32_t i32Q)
    __attribute__((alias("gj_q16_to_int_round")));
