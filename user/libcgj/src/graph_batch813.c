/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch813: freestanding sum of an int64_t array.
 *
 * Surface (unique symbols):
 *   int64_t gj_i64_sum_arr(const int64_t *a, size_t n);
 *     — Return a[0] + ... + a[n-1] as int64_t (wraps mod 2^64 two's
 *       complement). a == NULL or n == 0 → 0.
 *   int64_t __gj_i64_sum_arr  (alias)
 *   __libcgj_batch813_marker = "libcgj-batch813"
 *
 * Distinct from gj_u32_sum (batch609) / gj_u16_sum (batch638) /
 * gj_i32_sum (batch641) — unique gj_i64_sum_arr surface only; no
 * multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch813_marker[] = "libcgj-batch813";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i64_sum_arr — sum n int64_t elements (wrapping add).
 *
 * a: base (NULL with n > 0 → return 0)
 * n: element count
 *
 * Returns the sum of a[0..n) as int64_t (0 if a is NULL or n is 0).
 */
int64_t
gj_i64_sum_arr(const int64_t *pA, size_t n)
{
	size_t i;
	int64_t i64Sum;

	if (n == 0u || pA == NULL) {
		return 0;
	}

	i64Sum = 0;
	for (i = 0u; i < n; i++) {
		i64Sum = (int64_t)((uint64_t)i64Sum + (uint64_t)pA[i]);
	}
	return i64Sum;
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_i64_sum_arr(const int64_t *pA, size_t n)
    __attribute__((alias("gj_i64_sum_arr")));
