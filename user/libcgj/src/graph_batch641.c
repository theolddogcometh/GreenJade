/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch641: freestanding sum of an int32_t array
 * into an int64_t accumulator.
 *
 * Surface (unique symbols):
 *   int64_t gj_i32_sum(const int32_t *a, size_t n);
 *     — Return a[0] + ... + a[n-1] as int64_t. NULL a with n > 0
 *       yields 0; n == 0 yields 0. Widens each element to int64_t
 *       before add so partial sums do not wrap at 32 bits and
 *       negative elements contribute correctly.
 *   int64_t __gj_i32_sum  (alias)
 *   __libcgj_batch641_marker = "libcgj-batch641"
 *
 * Distinct from gj_u32_sum / __gj_u32_sum (graph_batch609) — do not
 * multi-def those symbols here.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch641_marker[] = "libcgj-batch641";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i32_sum — sum n int32_t elements into an int64_t.
 *
 * a: base (NULL with n > 0 → return 0)
 * n: element count
 *
 * Returns the sum of a[0..n) as int64_t (0 if a is NULL or n is 0).
 * Each element is widened to int64_t before addition.
 */
int64_t
gj_i32_sum(const int32_t *pA, size_t cN)
{
	size_t i;
	int64_t i64Sum;

	if (cN == 0u || pA == NULL) {
		return 0;
	}

	i64Sum = 0;
	for (i = 0u; i < cN; i++) {
		i64Sum += (int64_t)pA[i];
	}
	return i64Sum;
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_i32_sum(const int32_t *pA, size_t cN)
    __attribute__((alias("gj_i32_sum")));
