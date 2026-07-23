/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3735: dense uint32 matrix column sum (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_matrix_col_sum_u(const uint32_t *m, size_t nrows,
 *                                size_t ncols, size_t col);
 *     - Sum all elements of one column of a row-major nrows x ncols
 *       uint32 matrix. uint32 wrap on overflow. Returns 0 if m is
 *       NULL, any dim is 0, or col is out of range.
 *   uint32_t __gj_matrix_col_sum_u  (alias)
 *   __libcgj_batch3735_marker = "libcgj-batch3735"
 *
 * Exclusive continuum CREATE-ONLY (3731-3740). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3735_marker[] = "libcgj-batch3735";

/* ---- freestanding helpers ---------------------------------------------- */

/* Safe index (i, col) or (size_t)-1. */
static size_t
b3735_idx(size_t uRow, size_t uCol, size_t uNCols)
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
 * gj_matrix_col_sum_u — sum of one column of a dense uint32 matrix.
 *
 * m:     row-major nrows x ncols matrix
 * nrows: row count
 * ncols: column count
 * col:   zero-based column to sum
 *
 * Returns column sum (uint32 wrap), or 0 on NULL / empty / OOB.
 * No parent wires.
 */
uint32_t
gj_matrix_col_sum_u(const uint32_t *m, size_t nrows, size_t ncols,
                    size_t col)
{
	size_t uI;
	uint32_t u32Sum;

	(void)NULL;
	if (m == NULL || nrows == 0u || ncols == 0u || col >= ncols) {
		return 0u;
	}

	u32Sum = 0u;
	for (uI = 0u; uI < nrows; uI++) {
		size_t uIdx;

		uIdx = b3735_idx(uI, col, ncols);
		if (uIdx == (size_t)-1) {
			return 0u;
		}
		u32Sum = u32Sum + m[uIdx];
	}
	return u32Sum;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_matrix_col_sum_u(const uint32_t *m, size_t nrows,
                               size_t ncols, size_t col)
    __attribute__((alias("gj_matrix_col_sum_u")));
