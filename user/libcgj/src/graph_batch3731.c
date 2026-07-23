/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3731: row-major matrix linear index (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_matrix_idx_u(size_t row, size_t col, size_t ncols);
 *     - Row-major linear index of (row, col) in a matrix with ncols
 *       columns: row * ncols + col. Returns (size_t)-1 if ncols is 0
 *       or the product would overflow size_t.
 *   size_t __gj_matrix_idx_u  (alias)
 *   __libcgj_batch3731_marker = "libcgj-batch3731"
 *
 * Exclusive continuum CREATE-ONLY (3731-3740: matrix_idx_u,
 * matrix_get_u32_u, matrix_set_u32_u, matrix_row_sum_u,
 * matrix_col_sum_u, matrix_trace_u, matrix_identity_u,
 * matrix_transpose_u, matrix_add_u, batch_id_3740). Distinct from
 * fixed-size mat2/mat3 and gj_mat_*_i32 surfaces — unique gj_matrix_*_u
 * names; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3731_marker[] = "libcgj-batch3731";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Row-major index with overflow guard.
 * Invalid / overflow → (size_t)-1.
 */
static size_t
b3731_idx(size_t uRow, size_t uCol, size_t uNCols)
{
	size_t uBase;

	if (uNCols == 0u) {
		return (size_t)-1;
	}
	/* Guard uRow * uNCols overflow. */
	if (uRow != 0u && uRow > ((size_t)-1) / uNCols) {
		return (size_t)-1;
	}
	uBase = uRow * uNCols;
	/* Guard base + col overflow. */
	if (uBase > ((size_t)-1) - uCol) {
		return (size_t)-1;
	}
	return uBase + uCol;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_matrix_idx_u — row-major linear index of (row, col).
 *
 * row:   zero-based row
 * col:   zero-based column
 * ncols: number of columns in the matrix
 *
 * Returns row*ncols+col, or (size_t)-1 on invalid dims / overflow.
 * Does not allocate. No parent wires.
 */
size_t
gj_matrix_idx_u(size_t row, size_t col, size_t ncols)
{
	(void)NULL;
	return b3731_idx(row, col, ncols);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_matrix_idx_u(size_t row, size_t col, size_t ncols)
    __attribute__((alias("gj_matrix_idx_u")));
