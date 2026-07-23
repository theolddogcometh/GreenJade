/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3628: saturating int32_t divide (div_sat_u).
 *
 * Surface (unique symbols):
 *   int32_t gj_i32_div_sat_u(int32_t a, int32_t b);
 *     - a / b with C truncating division toward zero.
 *       b == 0 → 0.
 *       a == INT32_MIN && b == -1 → INT32_MAX (no UB / no wrap).
 *   int32_t __gj_i32_div_sat_u  (alias)
 *   __libcgj_batch3628_marker = "libcgj-batch3628"
 *
 * CREATE-ONLY exclusive continuum wave (3621-3630). Unique
 * gj_i32_div_sat_u surface only; no multi-def. Distinct from
 * gj_q16_div_sat (batch1472). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3628_marker[] = "libcgj-batch3628";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Saturating a / b in the int32_t domain.
 * Only overflow pair is INT32_MIN / -1 (two's-complement).
 */
static int32_t
b3628_div_sat(int32_t i32A, int32_t i32B)
{
	if (i32B == 0) {
		return 0;
	}
	if (i32A == INT32_MIN && i32B == -1) {
		return INT32_MAX;
	}
	return i32A / i32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i32_div_sat_u - signed 32-bit divide; clamp INT32_MIN/-1 to INT32_MAX.
 *
 * a: dividend
 * b: divisor (0 → 0)
 *
 * No parent wires.
 */
int32_t
gj_i32_div_sat_u(int32_t i32A, int32_t i32B)
{
	(void)NULL;
	return b3628_div_sat(i32A, i32B);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_i32_div_sat_u(int32_t i32A, int32_t i32B)
    __attribute__((alias("gj_i32_div_sat_u")));
