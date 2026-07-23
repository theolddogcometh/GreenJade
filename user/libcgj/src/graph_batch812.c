/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch812: freestanding maximum of an int64_t array.
 *
 * Surface (unique symbols):
 *   int64_t gj_i64_max_arr(const int64_t *a, size_t n);
 *     — Return the maximum of a[0..n). a == NULL or n == 0 → 0.
 *   int64_t __gj_i64_max_arr  (alias)
 *   __libcgj_batch812_marker = "libcgj-batch812"
 *
 * Distinct from gj_max_arr_i64 (batch410) / gj_i32_max (batch643) —
 * unique gj_i64_max_arr surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch812_marker[] = "libcgj-batch812";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i64_max_arr — greatest value in a[0..n).
 *
 * a: base (NULL → 0)
 * n: element count (0 → 0)
 *
 * Pure signed integer compare; no float.
 */
int64_t
gj_i64_max_arr(const int64_t *pA, size_t n)
{
	size_t i;
	int64_t i64Max;

	if (pA == NULL || n == 0u) {
		return 0;
	}

	i64Max = pA[0];
	for (i = 1u; i < n; i++) {
		if (pA[i] > i64Max) {
			i64Max = pA[i];
		}
	}
	return i64Max;
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_i64_max_arr(const int64_t *pA, size_t n)
    __attribute__((alias("gj_i64_max_arr")));
