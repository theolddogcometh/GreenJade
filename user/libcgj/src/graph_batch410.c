/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch410: maximum element of an int64_t array.
 *
 * Surface (unique symbols):
 *   int64_t gj_max_arr_i64(const int64_t *a, size_t n);
 *     — Return the maximum of a[0..n). a == NULL or n == 0 → 0.
 *   int64_t __gj_max_arr_i64  (alias)
 *   __libcgj_batch410_marker = "libcgj-batch410"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch410_marker[] = "libcgj-batch410";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_max_arr_i64 — greatest value in a[0..n).
 *
 * Edge cases:
 *   a == NULL → 0
 *   n == 0 → 0
 *   all elements equal → that value
 */
int64_t
gj_max_arr_i64(const int64_t *a, size_t n)
{
	size_t i;
	int64_t i64Max;

	if (a == NULL || n == 0u) {
		return 0;
	}

	i64Max = a[0];
	for (i = 1u; i < n; i++) {
		if (a[i] > i64Max) {
			i64Max = a[i];
		}
	}
	return i64Max;
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_max_arr_i64(const int64_t *a, size_t n)
    __attribute__((alias("gj_max_arr_i64")));
