/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3733: dense uint32 matrix element set (_u).
 *
 * Surface (unique symbols):
 *   int gj_matrix_set_u32_u(uint32_t *m, size_t nrows, size_t ncols,
 *                           size_t row, size_t col, uint32_t v);
 *     - Store v at m[row * ncols + col] in a row-major nrows x ncols
 *       uint32 matrix. Returns 1 on success, 0 if m is NULL, any dim
 *       is 0, or (row,col) is out of range / index would overflow.
 *   int __gj_matrix_set_u32_u  (alias)
 *   __libcgj_batch3733_marker = "libcgj-batch3733"
 *
 * Exclusive continuum CREATE-ONLY (3731-3740). Distinct from
 * gj_bitmat8_set and fixed mat2/mat3 surfaces — unique name; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3733_marker[] = "libcgj-batch3733";

/* ---- freestanding helpers ---------------------------------------------- */

/* Safe row-major index or (size_t)-1. */
static size_t
b3733_idx(size_t uRow, size_t uCol, size_t uNCols)
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
 * gj_matrix_set_u32_u — write one uint32 element into a dense matrix.
 *
 * m:     row-major nrows x ncols matrix (may be NULL → 0)
 * nrows: row count
 * ncols: column count
 * row:   zero-based row
 * col:   zero-based column
 * v:     value to store
 *
 * Returns 1 on write, 0 on NULL / empty / OOB. No parent wires.
 */
int
gj_matrix_set_u32_u(uint32_t *m, size_t nrows, size_t ncols, size_t row,
                    size_t col, uint32_t v)
{
	size_t uIdx;

	(void)NULL;
	if (m == NULL || nrows == 0u || ncols == 0u) {
		return 0;
	}
	if (row >= nrows || col >= ncols) {
		return 0;
	}
	uIdx = b3733_idx(row, col, ncols);
	if (uIdx == (size_t)-1) {
		return 0;
	}
	m[uIdx] = v;
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_matrix_set_u32_u(uint32_t *m, size_t nrows, size_t ncols,
                          size_t row, size_t col, uint32_t v)
    __attribute__((alias("gj_matrix_set_u32_u")));
