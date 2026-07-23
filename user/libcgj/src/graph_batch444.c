/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch444: n x n int32 matrix trace (row-major).
 *
 * Surface (unique symbols):
 *   int32_t gj_mat_trace_i32(const int32_t *A, size_t n);
 *     — Return sum of diagonal: sum_{i=0..n-1} A[i*n + i].
 *       int32 wrap on overflow. Returns 0 if A is NULL or n is 0.
 *       Distinct from gj_matmul_i32 (batch226) and gj_mat_det_i64
 *       (batch253).
 *   int32_t __gj_mat_trace_i32  (alias)
 *   __libcgj_batch444_marker = "libcgj-batch444"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch444_marker[] = "libcgj-batch444";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mat_trace_i32 — sum of diagonal of square int32 matrix, row-major.
 *
 * A: n x n matrix (row-major)
 * n: dimension
 *
 * Returns diagonal sum (int32 wrap), or 0 on NULL / empty.
 */
int32_t
gj_mat_trace_i32(const int32_t *pA, size_t n)
{
	int32_t i32Sum;
	size_t i;

	if (pA == NULL || n == 0u) {
		return 0;
	}

	i32Sum = 0;
	for (i = 0u; i < n; i++) {
		i32Sum = i32Sum + pA[i * n + i];
	}

	return i32Sum;
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_mat_trace_i32(const int32_t *pA, size_t n)
    __attribute__((alias("gj_mat_trace_i32")));
