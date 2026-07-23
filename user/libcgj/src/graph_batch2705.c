/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2705: absolute value of int32_t as uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_i32_abs_u(int32_t x);
 *     - Magnitude of x as an unsigned 32-bit value.
 *       Non-negative x maps to (uint32_t)x.
 *       Negative x maps to the two's-complement magnitude
 *       0u - (uint32_t)x, which for INT32_MIN yields 1u << 31
 *       (0x80000000), the only representable abs of that value
 *       in uint32_t.
 *   uint32_t __gj_i32_abs_u  (alias)
 *   __libcgj_batch2705_marker = "libcgj-batch2705"
 *
 * Post-2700 product/Steam integer helpers exclusive wave (2701-2710).
 * Distinct from gj_abs_i32 (batch838) and gj_i32_abs_arr (batch1212) —
 * unique gj_i32_abs_u surface only; no multi-def. Avoids signed
 * negation of INT32_MIN (undefined behaviour).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2705_marker[] = "libcgj-batch2705";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2705_abs_u(int32_t i32X)
{
	uint32_t u32X;

	u32X = (uint32_t)i32X;
	if (i32X < 0) {
		return 0u - u32X;
	}
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i32_abs_u — |x| as uint32_t; INT32_MIN -> 1u << 31.
 *
 * Unsigned wrap for the negative branch is defined and yields the
 * correct magnitude for every int32_t, including INT32_MIN.
 */
uint32_t
gj_i32_abs_u(int32_t i32X)
{
	(void)NULL;
	return b2705_abs_u(i32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_i32_abs_u(int32_t i32X)
    __attribute__((alias("gj_i32_abs_u")));
