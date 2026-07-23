/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch442: n x n int32 matrix scale in place (row-major).
 *
 * Surface (unique symbols):
 *   void gj_mat_scale_i32(int32_t *A, size_t n, int32_t s);
 *     — A = s * A for square n x n matrix stored row-major:
 *         A[i*n + j] = s * A[i*n + j]
 *       Element-wise int32 wrap on overflow. No-op if A is NULL or n is 0.
 *       Distinct from gj_matmul_i32 (batch226).
 *   void __gj_mat_scale_i32  (alias)
 *   __libcgj_batch442_marker = "libcgj-batch442"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch442_marker[] = "libcgj-batch442";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mat_scale_i32 — dense square int32 matrix scalar multiply, in place.
 *
 * A: n x n matrix (row-major), updated in place
 * n: dimension (n x n elements total)
 * s: scalar multiplier
 */
void
gj_mat_scale_i32(int32_t *pA, size_t n, int32_t i32S)
{
	size_t cElem;
	size_t i;

	if (pA == NULL || n == 0u) {
		return;
	}

	cElem = n * n;
	for (i = 0u; i < cElem; i++) {
		pA[i] = pA[i] * i32S;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_mat_scale_i32(int32_t *pA, size_t n, int32_t i32S)
    __attribute__((alias("gj_mat_scale_i32")));
