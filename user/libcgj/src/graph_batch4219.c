/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4219: three-way sign of int32_t (_s).
 *
 * Surface (unique symbols):
 *   int32_t gj_i32_sign_s(int32_t x);
 *     - Returns -1 if x < 0, 0 if x == 0, +1 if x > 0.
 *   int32_t __gj_i32_sign_s  (alias)
 *   __libcgj_batch4219_marker = "libcgj-batch4219"
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

const char __libcgj_batch4219_marker[] = "libcgj-batch4219";

/* ---- freestanding helpers ---------------------------------------------- */

/* Sign of iX as -1, 0, or +1. */
static int32_t
b4219_sign(int32_t iX)
{
	if (iX < 0) {
		return (int32_t)-1;
	}
	if (iX > 0) {
		return (int32_t)1;
	}
	return (int32_t)0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i32_sign_s - sign of x as -1, 0, or +1.
 *
 * Branch form avoids arithmetic right-shift assumptions and INT32_MIN
 * negation overflow; comparisons are exact for the full int32_t range.
 * Does not call libc.
 */
int32_t
gj_i32_sign_s(int32_t iX)
{
	(void)NULL;
	return b4219_sign(iX);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_i32_sign_s(int32_t iX)
    __attribute__((alias("gj_i32_sign_s")));
