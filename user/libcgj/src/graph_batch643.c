/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch643: freestanding maximum of an int32_t
 * array.
 *
 * Surface (unique symbols):
 *   int32_t gj_i32_max(const int32_t *a, size_t n);
 *     — Return the maximum of a[0..n). NULL a with n > 0 yields 0;
 *       n == 0 yields 0. Pure signed integer compare; no float.
 *   int32_t __gj_i32_max  (alias)
 *   __libcgj_batch643_marker = "libcgj-batch643"
 *
 * Distinct from gj_i32_min / __gj_i32_min (graph_batch642) and
 * gj_i32_sum / __gj_i32_sum (graph_batch641) — do not multi-def those
 * symbols here.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch643_marker[] = "libcgj-batch643";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i32_max — maximum of n int32_t elements.
 *
 * a: base (NULL with n > 0 → return 0)
 * n: element count
 *
 * Returns the maximum of a[0..n) (0 if a is NULL or n is 0).
 */
int32_t
gj_i32_max(const int32_t *a, size_t n)
{
	size_t i;
	int32_t m;

	if (n == 0u || a == NULL) {
		return 0;
	}

	m = a[0];
	for (i = 1u; i < n; i++) {
		if (a[i] > m) {
			m = a[i];
		}
	}
	return m;
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_i32_max(const int32_t *a, size_t n)
    __attribute__((alias("gj_i32_max")));
