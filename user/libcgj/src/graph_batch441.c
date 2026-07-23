/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch441: n x n int32 matrix add (row-major).
 *
 * Surface (unique symbols):
 *   void gj_mat_add_i32(const int32_t *A, const int32_t *B, int32_t *C,
 *                       size_t n);
 *     — C = A + B for square n x n matrices stored row-major:
 *         C[i*n + j] = A[i*n + j] + B[i*n + j]
 *       Element-wise int32 wrap on overflow. No-op if any pointer is NULL
 *       or n is 0. Distinct from gj_matmul_i32 (batch226).
 *   void __gj_mat_add_i32  (alias)
 *   __libcgj_batch441_marker = "libcgj-batch441"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch441_marker[] = "libcgj-batch441";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mat_add_i32 — dense square int32 matrix sum, row-major layout.
 *
 * A / B: input n x n matrices (row-major)
 * C:     output n x n matrix (row-major); may equal A or B (in-place OK)
 * n:     dimension (n x n elements total)
 */
void
gj_mat_add_i32(const int32_t *pA, const int32_t *pB, int32_t *pC, size_t n)
{
	size_t cElem;
	size_t i;

	if (pA == NULL || pB == NULL || pC == NULL || n == 0u) {
		return;
	}

	cElem = n * n;
	for (i = 0u; i < cElem; i++) {
		pC[i] = pA[i] + pB[i];
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_mat_add_i32(const int32_t *pA, const int32_t *pB, int32_t *pC,
                      size_t n)
    __attribute__((alias("gj_mat_add_i32")));
