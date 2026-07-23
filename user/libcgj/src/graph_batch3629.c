/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3629: saturating int64_t divide (div_sat_u).
 *
 * Surface (unique symbols):
 *   int64_t gj_i64_div_sat_u(int64_t a, int64_t b);
 *     - a / b with C truncating division toward zero.
 *       b == 0 → 0.
 *       a == INT64_MIN && b == -1 → INT64_MAX (no UB / no wrap).
 *   int64_t __gj_i64_div_sat_u  (alias)
 *   __libcgj_batch3629_marker = "libcgj-batch3629"
 *
 * CREATE-ONLY exclusive continuum wave (3621-3630). Unique
 * gj_i64_div_sat_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3629_marker[] = "libcgj-batch3629";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Saturating a / b in the int64_t domain.
 * Only overflow pair is INT64_MIN / -1 (two's-complement).
 */
static int64_t
b3629_div_sat(int64_t i64A, int64_t i64B)
{
	if (i64B == 0) {
		return 0;
	}
	if (i64A == INT64_MIN && i64B == -1) {
		return INT64_MAX;
	}
	return i64A / i64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i64_div_sat_u - signed 64-bit divide; clamp INT64_MIN/-1 to INT64_MAX.
 *
 * a: dividend
 * b: divisor (0 → 0)
 *
 * No parent wires.
 */
int64_t
gj_i64_div_sat_u(int64_t i64A, int64_t i64B)
{
	(void)NULL;
	return b3629_div_sat(i64A, i64B);
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_i64_div_sat_u(int64_t i64A, int64_t i64B)
    __attribute__((alias("gj_i64_div_sat_u")));
