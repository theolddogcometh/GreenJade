/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch925: saturating int64_t add.
 *
 * Surface (unique symbols):
 *   int64_t gj_i64_sat_add(int64_t a, int64_t b);
 *     — a + b with saturation at INT64_MAX / INT64_MIN (no wrap).
 *   int64_t __gj_i64_sat_add  (alias)
 *   __libcgj_batch925_marker = "libcgj-batch925"

 *
 * Does NOT define gj_sat_add_u64 / gj_u64_sat_add — signed path only.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch925_marker[] = "libcgj-batch925";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i64_sat_add — signed add; clamp to INT64_MAX/MIN on overflow.
 *
 * Overflow checks use the classical range tests so the add itself is only
 * performed when the mathematical sum is representable in int64_t (no UB).
 */
int64_t
gj_i64_sat_add(int64_t i64A, int64_t i64B)
{
	if (i64B > 0) {
		if (i64A > (INT64_MAX - i64B)) {
			return INT64_MAX;
		}
	} else if (i64B < 0) {
		if (i64A < (INT64_MIN - i64B)) {
			return INT64_MIN;
		}
	}
	return i64A + i64B;
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_i64_sat_add(int64_t i64A, int64_t i64B)
    __attribute__((alias("gj_i64_sat_add")));
