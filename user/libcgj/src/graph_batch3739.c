/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3739: dense uint32 square matrix add (_u).
 *
 * Surface (unique symbols):
 *   void gj_matrix_add_u(const uint32_t *a, const uint32_t *b,
 *                        uint32_t *c, size_t n);
 *     - c = a + b for square n x n uint32 matrices stored row-major:
 *         c[i*n + j] = a[i*n + j] + b[i*n + j]
 *       Element-wise uint32 wrap on overflow. No-op if any pointer is
 *       NULL or n is 0. c may equal a or b (in-place OK). No-op if
 *       n*n would overflow size_t.
 *   void __gj_matrix_add_u  (alias)
 *   __libcgj_batch3739_marker = "libcgj-batch3739"
 *
 * Exclusive continuum CREATE-ONLY (3731-3740). Distinct from
 * gj_mat_add_i32 (batch441) — unique uint32 _u surface; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3739_marker[] = "libcgj-batch3739";

/* ---- freestanding helpers ---------------------------------------------- */

/* Element-wise sum of n*n uint32 elements. */
static void
b3739_add(const uint32_t *pA, const uint32_t *pB, uint32_t *pC,
          size_t uElems)
{
	size_t uI;

	for (uI = 0u; uI < uElems; uI++) {
		pC[uI] = pA[uI] + pB[uI];
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_matrix_add_u — dense square uint32 matrix sum, row-major.
 *
 * a / b: input n x n matrices (row-major)
 * c:     output n x n matrix (row-major); may equal a or b
 * n:     dimension
 *
 * No-op on NULL / empty / overflow. No parent wires.
 */
void
gj_matrix_add_u(const uint32_t *a, const uint32_t *b, uint32_t *c,
                size_t n)
{
	size_t uElems;

	(void)NULL;
	if (a == NULL || b == NULL || c == NULL || n == 0u) {
		return;
	}
	if (n > ((size_t)-1) / n) {
		return;
	}
	uElems = n * n;
	b3739_add(a, b, c, uElems);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_matrix_add_u(const uint32_t *a, const uint32_t *b, uint32_t *c,
                       size_t n)
    __attribute__((alias("gj_matrix_add_u")));
