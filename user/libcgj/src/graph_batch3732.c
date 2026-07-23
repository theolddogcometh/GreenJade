/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3732: dense uint32 matrix element get (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_matrix_get_u32_u(const uint32_t *m, size_t nrows,
 *                                size_t ncols, size_t row, size_t col);
 *     - Read m[row * ncols + col] from a row-major nrows x ncols uint32
 *       matrix. Returns 0 if m is NULL, any dim is 0, or (row,col) is
 *       out of range / index would overflow.
 *   uint32_t __gj_matrix_get_u32_u  (alias)
 *   __libcgj_batch3732_marker = "libcgj-batch3732"
 *
 * Exclusive continuum CREATE-ONLY (3731-3740). Distinct from
 * gj_bitmat8_get and fixed mat2/mat3 surfaces — unique name; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3732_marker[] = "libcgj-batch3732";

/* ---- freestanding helpers ---------------------------------------------- */

/* Safe row-major index or (size_t)-1. */
static size_t
b3732_idx(size_t uRow, size_t uCol, size_t uNCols)
{
	size_t uBase;

	if (uNCols == 0u) {
		return (size_t)-1;
	}
	if (uRow != 0u && uRow > ((size_t)-1) / uNCols) {
		return (size_t)-1;
	}
	uBase = uRow * uNCols;
	if (uBase > ((size_t)-1) - uCol) {
		return (size_t)-1;
	}
	return uBase + uCol;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_matrix_get_u32_u — read one uint32 element from a dense matrix.
 *
 * m:     row-major nrows x ncols matrix (may be NULL → 0)
 * nrows: row count
 * ncols: column count
 * row:   zero-based row
 * col:   zero-based column
 *
 * Returns the element, or 0 on NULL / empty / OOB. No parent wires.
 */
uint32_t
gj_matrix_get_u32_u(const uint32_t *m, size_t nrows, size_t ncols,
                    size_t row, size_t col)
{
	size_t uIdx;

	(void)NULL;
	if (m == NULL || nrows == 0u || ncols == 0u) {
		return 0u;
	}
	if (row >= nrows || col >= ncols) {
		return 0u;
	}
	uIdx = b3732_idx(row, col, ncols);
	if (uIdx == (size_t)-1) {
		return 0u;
	}
	return m[uIdx];
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_matrix_get_u32_u(const uint32_t *m, size_t nrows,
                               size_t ncols, size_t row, size_t col)
    __attribute__((alias("gj_matrix_get_u32_u")));
