/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1375: saturating int64_t add (continuum name).
 *
 * Surface (unique symbols):
 *   int64_t gj_i64_saturating_add(int64_t a, int64_t b);
 *     — a + b with saturation at INT64_MAX / INT64_MIN (no wrap).
 *   int64_t __gj_i64_saturating_add  (alias)
 *   __libcgj_batch1375_marker = "libcgj-batch1375"
 *
 * Distinct from gj_i64_sat_add (batch925). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1375_marker[] = "libcgj-batch1375";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Return 1 if a + b is not representable in int64_t.
 * Range tests only — the sum itself is never formed on overflow.
 */
static int
b1375_add_overflows(int64_t i64A, int64_t i64B)
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
 * gj_i64_saturating_add — signed add; clamp to INT64_MAX/MIN on overflow.
 */
int64_t
gj_i64_saturating_add(int64_t i64A, int64_t i64B)
{
	if (b1375_add_overflows(i64A, i64B)) {
		return (i64B > 0) ? INT64_MAX : INT64_MIN;
	}
	return i64A + i64B;
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_i64_saturating_add(int64_t i64A, int64_t i64B)
    __attribute__((alias("gj_i64_saturating_add")));
