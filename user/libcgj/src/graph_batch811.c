/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch811: freestanding minimum of an int64_t array.
 *
 * Surface (unique symbols):
 *   int64_t gj_i64_min_arr(const int64_t *a, size_t n);
 *     — Return the minimum of a[0..n). a == NULL or n == 0 → 0.
 *   int64_t __gj_i64_min_arr  (alias)
 *   __libcgj_batch811_marker = "libcgj-batch811"
 *
 * Distinct from gj_min_arr_i64 (batch409) / gj_i32_min (batch642) —
 * unique gj_i64_min_arr surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch811_marker[] = "libcgj-batch811";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i64_min_arr — least value in a[0..n).
 *
 * a: base (NULL → 0)
 * n: element count (0 → 0)
 *
 * Pure signed integer compare; no float.
 */
int64_t
gj_i64_min_arr(const int64_t *pA, size_t n)
{
	size_t i;
	int64_t i64Min;

	if (pA == NULL || n == 0u) {
		return 0;
	}

	i64Min = pA[0];
	for (i = 1u; i < n; i++) {
		if (pA[i] < i64Min) {
			i64Min = pA[i];
		}
	}
	return i64Min;
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_i64_min_arr(const int64_t *pA, size_t n)
    __attribute__((alias("gj_i64_min_arr")));
