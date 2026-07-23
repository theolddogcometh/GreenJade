/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3738: dense uint32 square transpose (_u).
 *
 * Surface (unique symbols):
 *   void gj_matrix_transpose_u(const uint32_t *a, uint32_t *b, size_t n);
 *     - b = a^T for square n x n uint32 matrices stored row-major:
 *         b[j*n + i] = a[i*n + j]
 *       No-op if any pointer is NULL or n is 0. a and b must not alias
 *       (out-of-place only).
 *   void __gj_matrix_transpose_u  (alias)
 *   __libcgj_batch3738_marker = "libcgj-batch3738"
 *
 * Exclusive continuum CREATE-ONLY (3731-3740). Distinct from
 * gj_mat_transpose_i32 / gj_mat2_i32_transpose — unique uint32 _u
 * surface; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3738_marker[] = "libcgj-batch3738";

/* ---- freestanding helpers ---------------------------------------------- */

/* Out-of-place square transpose, row-major. */
static void
b3738_transpose(const uint32_t *pA, uint32_t *pB, size_t uN)
{
	size_t uI;
	size_t uJ;

	for (uI = 0u; uI < uN; uI++) {
		for (uJ = 0u; uJ < uN; uJ++) {
			pB[uJ * uN + uI] = pA[uI * uN + uJ];
		}
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_matrix_transpose_u — dense square uint32 transpose, row-major.
 *
 * a: input n x n matrix (row-major)
 * b: output n x n matrix (row-major); must not overlap a
 * n: dimension
 *
 * No-op on NULL / empty. No parent wires.
 */
void
gj_matrix_transpose_u(const uint32_t *a, uint32_t *b, size_t n)
{
	(void)NULL;
	if (a == NULL || b == NULL || n == 0u) {
		return;
	}
	b3738_transpose(a, b, n);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_matrix_transpose_u(const uint32_t *a, uint32_t *b, size_t n)
    __attribute__((alias("gj_matrix_transpose_u")));
