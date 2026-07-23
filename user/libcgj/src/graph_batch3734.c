/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3734: dense uint32 matrix row sum (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_matrix_row_sum_u(const uint32_t *m, size_t nrows,
 *                                size_t ncols, size_t row);
 *     - Sum all elements of one row of a row-major nrows x ncols
 *       uint32 matrix. uint32 wrap on overflow. Returns 0 if m is
 *       NULL, any dim is 0, or row is out of range.
 *   uint32_t __gj_matrix_row_sum_u  (alias)
 *   __libcgj_batch3734_marker = "libcgj-batch3734"
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

const char __libcgj_batch3734_marker[] = "libcgj-batch3734";

/* ---- freestanding helpers ---------------------------------------------- */

/* Base index of row or (size_t)-1 on overflow / empty. */
static size_t
b3734_row_base(size_t uRow, size_t uNCols)
{
	if (uNCols == 0u) {
		return (size_t)-1;
	}
	if (uRow != 0u && uRow > ((size_t)-1) / uNCols) {
		return (size_t)-1;
	}
	return uRow * uNCols;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_matrix_row_sum_u — sum of one row of a dense uint32 matrix.
 *
 * m:     row-major nrows x ncols matrix
 * nrows: row count
 * ncols: column count
 * row:   zero-based row to sum
 *
 * Returns row sum (uint32 wrap), or 0 on NULL / empty / OOB.
 * No parent wires.
 */
uint32_t
gj_matrix_row_sum_u(const uint32_t *m, size_t nrows, size_t ncols,
                    size_t row)
{
	size_t uBase;
	size_t uJ;
	uint32_t u32Sum;

	(void)NULL;
	if (m == NULL || nrows == 0u || ncols == 0u || row >= nrows) {
		return 0u;
	}
	uBase = b3734_row_base(row, ncols);
	if (uBase == (size_t)-1) {
		return 0u;
	}

	u32Sum = 0u;
	for (uJ = 0u; uJ < ncols; uJ++) {
		u32Sum = u32Sum + m[uBase + uJ];
	}
	return u32Sum;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_matrix_row_sum_u(const uint32_t *m, size_t nrows,
                               size_t ncols, size_t row)
    __attribute__((alias("gj_matrix_row_sum_u")));
