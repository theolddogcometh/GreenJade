/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch226: n x n int32 matrix multiply (row-major).
 *
 * Surface (unique symbols):
 *   void gj_matmul_i32(const int32_t *A, const int32_t *B, int32_t *C,
 *                      size_t n);
 *     — C = A * B for square n x n matrices stored row-major:
 *         C[i*n + j] = sum_{k=0..n-1} A[i*n + k] * B[k*n + j]
 *       Products accumulate in int64_t then narrow to int32_t (wrap on
 *       overflow of the final store). No-op if any pointer is NULL or
 *       n is 0. Matrices may not alias (C must not overlap A or B).
 *   __gj_matmul_i32  (alias)
 *   __libcgj_batch226_marker = "libcgj-batch226"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No malloc, no errno. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch226_marker[] = "libcgj-batch226";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_matmul_i32 — dense square int32 matrix product, row-major layout.
 *
 * Triple loop i/k/j with ik-fixed A load for better locality on B/C
 * column walks. Intermediate sum is int64_t so partial products do not
 * wrap before the final narrow store to C.
 */
void
gj_matmul_i32(const int32_t *A, const int32_t *B, int32_t *C, size_t n)
{
	size_t i;
	size_t j;
	size_t k;

	if (A == NULL || B == NULL || C == NULL || n == 0u) {
		return;
	}

	for (i = 0u; i < n; i++) {
		for (j = 0u; j < n; j++) {
			C[i * n + j] = 0;
		}
	}

	for (i = 0u; i < n; i++) {
		for (k = 0u; k < n; k++) {
			int64_t aik = (int64_t)A[i * n + k];

			for (j = 0u; j < n; j++) {
				int64_t acc;

				acc = (int64_t)C[i * n + j] +
				      aik * (int64_t)B[k * n + j];
				C[i * n + j] = (int32_t)acc;
			}
		}
	}
}

void __gj_matmul_i32(const int32_t *A, const int32_t *B, int32_t *C, size_t n)
    __attribute__((alias("gj_matmul_i32")));
