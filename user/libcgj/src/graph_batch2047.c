/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2047: three-way compare of int64_t.
 *
 * Surface (unique symbols):
 *   int gj_i64_cmp(int64_t a, int64_t b);
 *     - Returns -1 if a < b, 0 if a == b, +1 if a > b.
 *   int __gj_i64_cmp  (alias)
 *   __libcgj_batch2047_marker = "libcgj-batch2047"
 *
 * Post-2000 i64 arith exclusive wave (2041-2050). Unique gj_i64_cmp
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2047_marker[] = "libcgj-batch2047";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b2047_cmp - three-way comparison of a against b.
 */
static int
b2047_cmp(int64_t i64A, int64_t i64B)
{
	if (i64A < i64B) {
		return -1;
	}
	if (i64A > i64B) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i64_cmp - three-way compare: -1 / 0 / +1 for a < b / a == b / a > b.
 *
 * Branch form; no subtraction (avoids overflow when comparing extremes).
 */
int
gj_i64_cmp(int64_t i64A, int64_t i64B)
{
	(void)NULL;
	return b2047_cmp(i64A, i64B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_i64_cmp(int64_t i64A, int64_t i64B)
    __attribute__((alias("gj_i64_cmp")));
