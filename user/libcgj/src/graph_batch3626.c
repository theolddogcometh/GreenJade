/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3626: saturating int32_t multiply (mul_sat_u).
 *
 * Surface (unique symbols):
 *   int32_t gj_i32_mul_sat_u(int32_t a, int32_t b);
 *     - a * b with saturation at INT32_MAX / INT32_MIN (no wrap, no UB).
 *   int32_t __gj_i32_mul_sat_u  (alias)
 *   __libcgj_batch3626_marker = "libcgj-batch3626"
 *
 * CREATE-ONLY exclusive continuum wave (3621-3630). Unique
 * gj_i32_mul_sat_u surface only; no multi-def. Product formed in
 * int64_t (always exact for int32 factors) then clamped — no __int128.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3626_marker[] = "libcgj-batch3626";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Saturating a * b in the int32_t domain via exact int64 product.
 * int32 * int32 always fits in int64_t, so no overflow on the widen.
 */
static int32_t
b3626_mul_sat(int32_t i32A, int32_t i32B)
{
	int64_t i64P;

	i64P = (int64_t)i32A * (int64_t)i32B;
	if (i64P > (int64_t)INT32_MAX) {
		return INT32_MAX;
	}
	if (i64P < (int64_t)INT32_MIN) {
		return INT32_MIN;
	}
	return (int32_t)i64P;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i32_mul_sat_u - signed 32-bit multiply; clamp to INT32_MAX/MIN.
 *
 * a, b: factors
 *
 * Zero factors are exact (product 0). Overflow direction follows the
 * sign of the mathematical product. No parent wires.
 */
int32_t
gj_i32_mul_sat_u(int32_t i32A, int32_t i32B)
{
	(void)NULL;
	return b3626_mul_sat(i32A, i32B);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_i32_mul_sat_u(int32_t i32A, int32_t i32B)
    __attribute__((alias("gj_i32_mul_sat_u")));
