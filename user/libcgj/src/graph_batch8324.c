/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8324: qsort/bsearch i32 signed three-way cmp.
 *
 * Surface (unique symbols):
 *   int gj_cmp_i32_sign_8324(int32_t a, int32_t b);
 *     - Sign of (a vs b): -1 if a < b, 0 if a == b, +1 if a > b.
 *       Overflow-safe (no a - b). Signed integer sort/search helper.
 *   int __gj_cmp_i32_sign_8324  (alias)
 *   __libcgj_batch8324_marker = "libcgj-batch8324"
 *
 * Exclusive continuum CREATE-ONLY (8321-8330: qsort/bsearch compare
 * stubs). Unique gj_cmp_i32_sign_8324 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8324_marker[] = "libcgj-batch8324";

/* ---- freestanding helpers ---------------------------------------------- */

/* Sign of ordered relation between signed i32 values. */
static int
b8324_sign(int32_t i32A, int32_t i32B)
{
	if (i32A < i32B) {
		return -1;
	}
	if (i32A > i32B) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cmp_i32_sign_8324 - signed three-way i32 compare (sign of order).
 *
 * a, b: signed values under comparison
 *
 * Returns -1 / 0 / +1. Safe at INT32_MIN / INT32_MAX (no subtraction).
 * No parent wires.
 */
int
gj_cmp_i32_sign_8324(int32_t i32A, int32_t i32B)
{
	(void)NULL;
	return b8324_sign(i32A, i32B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_cmp_i32_sign_8324(int32_t i32A, int32_t i32B)
    __attribute__((alias("gj_cmp_i32_sign_8324")));
