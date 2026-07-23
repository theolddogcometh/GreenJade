/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch254: CSR sparse matrix-vector multiply (int64).
 *
 * Surface (unique symbols):
 *   void gj_spmv_csr_i64(size_t nrows,
 *                        const size_t *row_ptr,
 *                        const size_t *col_idx,
 *                        const int64_t *vals,
 *                        const int64_t *x,
 *                        int64_t *y);
 *     — y = A * x for A stored in Compressed Sparse Row form:
 *         row_ptr[0..nrows]  (length nrows+1; row i spans [row_ptr[i],
 *                             row_ptr[i+1]))
 *         col_idx[j], vals[j] for each nonzero j
 *       Writes y[i] = sum_j vals[j] * x[col_idx[j]] over the row span.
 *       Integer multiply/add with natural int64 wrap on overflow.
 *       If any critical pointer is NULL, returns silently (y unchanged).
 *   __gj_spmv_csr_i64  (alias)
 *   __libcgj_batch254_marker = "libcgj-batch254"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch254_marker[] = "libcgj-batch254";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b254_spmv_row — accumulate one CSR row into an int64 sum.
 *
 * Row nonzeros live in [iBeg, iEnd) of col_idx/vals. x is the dense
 * right-hand vector. Returns the row dot product (wrap on overflow).
 */
static int64_t
b254_spmv_row(size_t iBeg, size_t iEnd, const size_t *pColIdx,
              const int64_t *pVals, const int64_t *pX)
{
	int64_t i64Sum = 0;
	size_t iNz;

	for (iNz = iBeg; iNz < iEnd; iNz++) {
		i64Sum += pVals[iNz] * pX[pColIdx[iNz]];
	}
	return i64Sum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_spmv_csr_i64 — CSR sparse matrix-vector product y = A x (int64).
 *
 * A is nrows × (implicit ncols) in CSR:
 *   for each row i in [0, nrows):
 *     y[i] = sum_{j = row_ptr[i] .. row_ptr[i+1]-1} vals[j] * x[col_idx[j]]
 *
 * Critical pointers: row_ptr, col_idx, vals, x, y. If any is NULL, return
 * without writing (y left unchanged). nrows == 0 is a no-op once checks
 * pass. Caller must ensure col indices are in range for x and that
 * row_ptr is length nrows+1 with non-decreasing entries.
 */
void
gj_spmv_csr_i64(size_t nrows, const size_t *row_ptr, const size_t *col_idx,
                const int64_t *vals, const int64_t *x, int64_t *y)
{
	size_t iRow;

	if (row_ptr == NULL || col_idx == NULL || vals == NULL || x == NULL ||
	    y == NULL) {
		return;
	}

	for (iRow = 0u; iRow < nrows; iRow++) {
		y[iRow] = b254_spmv_row(row_ptr[iRow], row_ptr[iRow + 1u],
		                        col_idx, vals, x);
	}
}

void __gj_spmv_csr_i64(size_t nrows, const size_t *row_ptr,
                       const size_t *col_idx, const int64_t *vals,
                       const int64_t *x, int64_t *y)
    __attribute__((alias("gj_spmv_csr_i64")));
