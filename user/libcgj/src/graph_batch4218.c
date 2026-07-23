/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4218: saturating absolute value of int32_t (_s).
 *
 * Surface (unique symbols):
 *   int32_t gj_i32_abs_s(int32_t x);
 *     - |x| as int32_t. INT32_MIN has no positive int32_t magnitude;
 *       this function saturates to INT32_MAX (not undefined negation).
 *   int32_t __gj_i32_abs_s  (alias)
 *   __libcgj_batch4218_marker = "libcgj-batch4218"
 *
 * Exclusive continuum CREATE-ONLY (4211-4220: u32_min3_u, u32_max3_u,
 * u64_min3_u, u64_max3_u, u32_clamp_range_u, u64_clamp_range_u,
 * i32_clamp_range_s, i32_abs_s, i32_sign_s, batch_id_4220).
 * Distinct surface in this wave; no parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4218_marker[] = "libcgj-batch4218";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * |x| with INT32_MIN → INT32_MAX saturation. Never evaluates -INT32_MIN.
 */
static int32_t
b4218_abs(int32_t iX)
{
	if (iX >= 0) {
		return iX;
	}
	if (iX == INT32_MIN) {
		return INT32_MAX;
	}
	return -iX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i32_abs_s - absolute value of x as int32_t (saturating).
 *
 * x: signed input
 *
 * For x > INT32_MIN and x < 0, returns -x without evaluating -INT32_MIN.
 * INT32_MIN saturates to INT32_MAX. Non-negative x is returned unchanged.
 * Does not call libc.
 */
int32_t
gj_i32_abs_s(int32_t iX)
{
	(void)NULL;
	return b4218_abs(iX);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_i32_abs_s(int32_t iX)
    __attribute__((alias("gj_i32_abs_s")));
