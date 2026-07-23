/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3622: saturating int32_t add (add_sat_u).
 *
 * Surface (unique symbols):
 *   int32_t gj_i32_add_sat_u(int32_t a, int32_t b);
 *     - a + b with saturation at INT32_MAX / INT32_MIN (no wrap, no UB).
 *   int32_t __gj_i32_add_sat_u  (alias)
 *   __libcgj_batch3622_marker = "libcgj-batch3622"
 *
 * CREATE-ONLY exclusive continuum wave (3621-3630). Unique
 * gj_i32_add_sat_u surface only; no multi-def. Distinct from
 * gj_i32_sat_add_u (batch2905) name order. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3622_marker[] = "libcgj-batch3622";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Saturating a + b in the int32_t domain. Range tests ensure the add
 * itself is only performed when the mathematical sum is representable.
 */
static int32_t
b3622_add_sat(int32_t i32A, int32_t i32B)
{
	if (i32B > 0) {
		if (i32A > (INT32_MAX - i32B)) {
			return INT32_MAX;
		}
	} else if (i32B < 0) {
		if (i32A < (INT32_MIN - i32B)) {
			return INT32_MIN;
		}
	}
	return i32A + i32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i32_add_sat_u - signed 32-bit add; clamp to INT32_MAX/MIN on overflow.
 *
 * a, b: addends
 *
 * Overflow high when b > 0 and a > INT32_MAX - b.
 * Overflow low when b < 0 and a < INT32_MIN - b.
 * No parent wires.
 */
int32_t
gj_i32_add_sat_u(int32_t i32A, int32_t i32B)
{
	(void)NULL;
	return b3622_add_sat(i32A, i32B);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_i32_add_sat_u(int32_t i32A, int32_t i32B)
    __attribute__((alias("gj_i32_add_sat_u")));
