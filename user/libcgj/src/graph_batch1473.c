/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1473: integer → Q16.16 conversion (saturating).
 *
 * Surface (unique symbols for this TU):
 *   int32_t gj_q16_from_int_sat(int v);
 *     — integer → Q16.16 (v << 16). Values outside the representable
 *       integer part of Q16.16 saturate to INT32_MIN / INT32_MAX.
 *   int32_t __gj_q16_from_int_sat  (alias)
 *   __libcgj_batch1473_marker = "libcgj-batch1473"
 *
 * Q16.16 layout (signed 32-bit): 1.0 == 0x00010000.
 * Representable integer part is roughly [-32768, 32767].
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1473_marker[] = "libcgj-batch1473";

/* Full integer range that shifts cleanly into signed Q16.16. */
#define B1473_Q16_INT_MAX  32767
#define B1473_Q16_INT_MIN  (-32768)

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_q16_from_int_sat — convert a signed integer to Q16.16 with saturation.
 *
 * |v| beyond the Q16.16 integer capacity clamps to INT32_MAX / INT32_MIN
 * rather than wrapping through the left shift.
 */
int32_t
gj_q16_from_int_sat(int nVal)
{
	if (nVal > B1473_Q16_INT_MAX) {
		return INT32_MAX;
	}
	if (nVal < B1473_Q16_INT_MIN) {
		return INT32_MIN;
	}
	return (int32_t)((int64_t)nVal << 16);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_q16_from_int_sat(int nVal)
    __attribute__((alias("gj_q16_from_int_sat")));
