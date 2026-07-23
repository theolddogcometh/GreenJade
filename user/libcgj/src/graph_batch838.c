/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch838: absolute value of int32_t.
 *
 * Surface (unique symbols):
 *   int32_t gj_abs_i32(int32_t x);
 *     — |x| for representable results. INT32_MIN has no positive
 *       int32_t magnitude; this function returns INT32_MIN unchanged
 *       (documented wrap of two's-complement abs). Does not invoke
 *       signed negation of INT32_MIN (undefined behaviour).
 *   int32_t __gj_abs_i32  (alias)
 *   __libcgj_batch838_marker = "libcgj-batch838"
 *
 * Distinct from gj_abs_i64 (batch378, returns uint64_t magnitude) —
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch838_marker[] = "libcgj-batch838";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_abs_i32 — |x| as int32_t; INT32_MIN → INT32_MIN (documented).
 *
 * For x > INT32_MIN and x < 0, returns -x without ever evaluating
 * -INT32_MIN. Non-negative x is returned unchanged.
 */
int32_t
gj_abs_i32(int32_t i32X)
{
	if (i32X >= 0) {
		return i32X;
	}
	if (i32X == INT32_MIN) {
		return INT32_MIN;
	}
	return -i32X;
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_abs_i32(int32_t i32X)
    __attribute__((alias("gj_abs_i32")));
