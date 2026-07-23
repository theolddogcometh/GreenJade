/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3623: saturating int64_t add
 * (exclusive _u2; rename on collision with batch2041 gj_i64_add_sat_u).
 *
 * Surface (unique symbols):
 *   int64_t gj_i64_add_sat_u2(int64_t a, int64_t b);
 *     - a + b with saturation at INT64_MAX / INT64_MIN (no wrap, no UB).
 *   int64_t __gj_i64_add_sat_u2  (alias)
 *   __libcgj_batch3623_marker = "libcgj-batch3623"
 *
 * CREATE-ONLY exclusive continuum wave (3621-3630). Unique
 * gj_i64_add_sat_u2 surface only; no multi-def. Distinct from
 * gj_i64_add_sat_u (batch2041) / gj_i64_sat_add_u (batch2907) /
 * gj_i64_sat_add (batch925). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3623_marker[] = "libcgj-batch3623";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b3623_add_ovf - 1 if a+b is not representable in int64_t.
 * Range tests only; the sum is never formed on overflow.
 */
static int
b3623_add_ovf(int64_t i64A, int64_t i64B)
{
	if (i64B > 0) {
		return (i64A > (INT64_MAX - i64B)) ? 1 : 0;
	}
	if (i64B < 0) {
		return (i64A < (INT64_MIN - i64B)) ? 1 : 0;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i64_add_sat_u2 - signed add; clamp to INT64_MAX/MIN on overflow.
 *
 * Self-contained reimplementation; no parent wires.
 */
int64_t
gj_i64_add_sat_u2(int64_t i64A, int64_t i64B)
{
	(void)NULL;
	if (b3623_add_ovf(i64A, i64B) != 0) {
		return (i64B > 0) ? INT64_MAX : INT64_MIN;
	}
	return i64A + i64B;
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_i64_add_sat_u2(int64_t i64A, int64_t i64B)
    __attribute__((alias("gj_i64_add_sat_u2")));
