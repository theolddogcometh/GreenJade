/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3866: saturating abs of u32-as-i32 bits (_u).
 *
 * Surface (unique symbols):
 *   int32_t gj_u32_abs_to_i32_u(uint32_t x);
 *     - Reinterpret x as two's-complement int32_t, then return |x|
 *       as int32_t. INT32_MIN saturates to INT32_MAX (no UB on
 *       negation of the most-negative value).
 *   int32_t __gj_u32_abs_to_i32_u  (alias)
 *   __libcgj_batch3866_marker = "libcgj-batch3866"
 *
 * Exclusive continuum CREATE-ONLY (3861-3870). Unique
 * gj_u32_abs_to_i32_u surface only; no multi-def. Distinct from
 * gj_u32_to_i32_sat_u (batch3869 saturate-cast) and
 * gj_i64_abs_sat_u (batch2045). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3866_marker[] = "libcgj-batch3866";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b3866_is_min - true iff the bit pattern is INT32_MIN
 * (abs not representable in int32_t).
 */
static int
b3866_is_min(int32_t i32X)
{
	return (i32X == INT32_MIN) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_abs_to_i32_u - |reinterpret_cast_i32(x)| with INT32_MIN sat.
 *
 * x: 32-bit pattern reinterpreted as signed int32_t
 *
 * Returns the absolute value in int32_t. INT32_MIN -> INT32_MAX.
 * No parent wires.
 */
int32_t
gj_u32_abs_to_i32_u(uint32_t u32X)
{
	int32_t i32X;

	(void)NULL;
	i32X = (int32_t)u32X;
	if (b3866_is_min(i32X) != 0) {
		return INT32_MAX;
	}
	if (i32X < 0) {
		return -i32X;
	}
	return i32X;
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_u32_abs_to_i32_u(uint32_t u32X)
    __attribute__((alias("gj_u32_abs_to_i32_u")));
