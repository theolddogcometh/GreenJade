/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3736: dense uint32 square matrix trace (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_matrix_trace_u(const uint32_t *m, size_t n);
 *     - Sum of the main diagonal of a row-major n x n uint32 matrix:
 *         sum_{i=0..n-1} m[i*n + i]
 *       uint32 wrap on overflow. Returns 0 if m is NULL or n is 0.
 *   uint32_t __gj_matrix_trace_u  (alias)
 *   __libcgj_batch3736_marker = "libcgj-batch3736"
 *
 * Exclusive continuum CREATE-ONLY (3731-3740). Distinct from
 * gj_mat_trace_i32 (batch444) — unique uint32 _u surface; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3736_marker[] = "libcgj-batch3736";

/* ---- freestanding helpers ---------------------------------------------- */

/* Diagonal sum of n x n row-major uint32 matrix. */
static uint32_t
b3736_trace(const uint32_t *pM, size_t uN)
{
	size_t uI;
	uint32_t u32Sum;

	u32Sum = 0u;
	for (uI = 0u; uI < uN; uI++) {
		/* i*n + i; n != 0 here; guard product overflow. */
		if (uI != 0u && uI > ((size_t)-1) / uN) {
			return 0u;
		}
		u32Sum = u32Sum + pM[uI * uN + uI];
	}
	return u32Sum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_matrix_trace_u — main-diagonal sum of a square uint32 matrix.
 *
 * m: row-major n x n matrix
 * n: dimension
 *
 * Returns diagonal sum (uint32 wrap), or 0 on NULL / empty.
 * No parent wires.
 */
uint32_t
gj_matrix_trace_u(const uint32_t *m, size_t n)
{
	(void)NULL;
	if (m == NULL || n == 0u) {
		return 0u;
	}
	return b3736_trace(m, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_matrix_trace_u(const uint32_t *m, size_t n)
    __attribute__((alias("gj_matrix_trace_u")));
