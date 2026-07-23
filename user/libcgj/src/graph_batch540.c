/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch540: int32 L2 squared distance (sum of squares).
 *
 * Surface (unique symbols):
 *   int64_t gj_l2sq_i32(const int32_t *a, const int32_t *b, size_t n);
 *     — sum_{i=0..n-1} (a[i] - b[i])^2 as int64_t. Differences are
 *       widened to int64 before multiply so int32 min/max pairs do not
 *       wrap the product. Returns 0 if a or b is NULL (or n is 0).
 *   __gj_l2sq_i32  (alias)
 *   __libcgj_batch540_marker = "libcgj-batch540"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch540_marker[] = "libcgj-batch540";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_l2sq_i32 — squared Euclidean (L2) distance between two int32 vectors.
 *
 * a, b: length-n int32 arrays (must both be non-NULL for a non-zero sum).
 * n:    number of elements; 0 yields 0.
 *
 * Each element difference is cast to int64_t before squaring so that
 * (INT32_MIN - INT32_MAX) and similar extremes stay in range for the
 * product; the running sum is also int64_t (wraps only if the total
 * overflows int64, which is accepted).
 */
int64_t
gj_l2sq_i32(const int32_t *a, const int32_t *b, size_t n)
{
	int64_t sum;
	size_t i;

	if (a == NULL || b == NULL) {
		return 0;
	}

	sum = 0;
	for (i = 0u; i < n; i++) {
		int64_t d;

		d = (int64_t)a[i] - (int64_t)b[i];
		sum += d * d;
	}
	return sum;
}

int64_t __gj_l2sq_i32(const int32_t *a, const int32_t *b, size_t n)
    __attribute__((alias("gj_l2sq_i32")));
