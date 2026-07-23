/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch443: n x n int32 matrix transpose (row-major).
 *
 * Surface (unique symbols):
 *   void gj_mat_transpose_i32(const int32_t *A, int32_t *B, size_t n);
 *     — B = A^T for square n x n matrices stored row-major:
 *         B[j*n + i] = A[i*n + j]
 *       No-op if any pointer is NULL or n is 0. A and B must not alias
 *       (out-of-place only). Distinct from gj_matmul_i32 (batch226).
 *   void __gj_mat_transpose_i32  (alias)
 *   __libcgj_batch443_marker = "libcgj-batch443"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch443_marker[] = "libcgj-batch443";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mat_transpose_i32 — dense square int32 transpose, row-major layout.
 *
 * A: input n x n matrix (row-major)
 * B: output n x n matrix (row-major); must not overlap A
 * n: dimension
 */
void
gj_mat_transpose_i32(const int32_t *pA, int32_t *pB, size_t n)
{
	size_t i;
	size_t j;

	if (pA == NULL || pB == NULL || n == 0u) {
		return;
	}

	for (i = 0u; i < n; i++) {
		for (j = 0u; j < n; j++) {
			pB[j * n + i] = pA[i * n + j];
		}
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_mat_transpose_i32(const int32_t *pA, int32_t *pB, size_t n)
    __attribute__((alias("gj_mat_transpose_i32")));
