/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch253: determinant of a small int64 square matrix
 * via Bareiss fraction-free Gaussian elimination (exact integer det).
 *
 * Surface (unique symbols):
 *   int gj_mat_det_i64(const int64_t *a, size_t n, int64_t *out_det);
 *     — a is row-major n×n. n must be in 0..8 inclusive.
 *       Copies into a local buffer, runs Bareiss with partial (nonzero)
 *       pivoting, writes the integer determinant to *out_det.
 *       Singular matrices yield *out_det = 0 (still success).
 *       n==0 → det 1 (empty product; a may be NULL).
 *       Returns 0 on success, -1 on bad args (NULL out_det, n>8,
 *       or NULL a with n>0).
 *   __gj_mat_det_i64  (alias)
 *   __libcgj_batch253_marker = "libcgj-batch253"
 *
 * Notes:
 *   - Rationale over rational GE: avoids fractions; Bareiss keeps exact
 *     integer intermediates with exact division at each step.
 *   - Intermediate products use int64_t (two's-complement wrap if entries
 *     are pathologically large for n=8); intended for modest integer matrices.
 *   - Clean-room freestanding pure C. Compiles with -ffreestanding -msse2.
 *     No malloc, no errno, no __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch253_marker[] = "libcgj-batch253";

enum { B253_NMAX = 8 };

/* ---- freestanding helpers ---------------------------------------------- */

/* Absolute value for pivot selection; INT64_MIN stays INT64_MIN (unsigned cast). */
static uint64_t
b253_uabs(int64_t v)
{
	if (v < 0) {
		return (uint64_t)(-(v + 1)) + 1ull;
	}
	return (uint64_t)v;
}

/* Swap rows r1 and r2 of n×n matrix m (row-major). */
static void
b253_swap_rows(int64_t *m, size_t n, size_t r1, size_t r2)
{
	size_t j;
	int64_t t;

	if (r1 == r2) {
		return;
	}
	for (j = 0u; j < n; j++) {
		t = m[r1 * n + j];
		m[r1 * n + j] = m[r2 * n + j];
		m[r2 * n + j] = t;
	}
}

/*
 * Bareiss fraction-free elimination on n×n matrix m (destroyed in place).
 * Returns the integer determinant. n >= 1, n <= B253_NMAX.
 *
 * At step k, for i,j > k:
 *   m[i][j] ← (m[k][k]*m[i][j] - m[i][k]*m[k][j]) / prev
 * where prev = 1 when k==0 else m[k-1][k-1] (exact division).
 * Row swaps for a nonzero pivot flip the sign.
 */
static int64_t
b253_bareiss(int64_t *m, size_t n)
{
	size_t k;
	size_t i;
	size_t j;
	size_t piv;
	int sign;
	int64_t prev;
	int64_t akk;
	int64_t num;
	uint64_t best;

	sign = 1;

	for (k = 0u; k + 1u < n; k++) {
		/* Partial pivot: largest |entry| in column k, rows k..n-1. */
		piv = k;
		best = b253_uabs(m[k * n + k]);
		for (i = k + 1u; i < n; i++) {
			uint64_t av = b253_uabs(m[i * n + k]);

			if (av > best) {
				best = av;
				piv = i;
			}
		}
		if (best == 0ull) {
			/* Entire column zero below diagonal → singular. */
			return 0;
		}
		if (piv != k) {
			b253_swap_rows(m, n, k, piv);
			sign = -sign;
		}

		akk = m[k * n + k];
		prev = (k == 0u) ? 1 : m[(k - 1u) * n + (k - 1u)];

		for (i = k + 1u; i < n; i++) {
			int64_t aik = m[i * n + k];

			for (j = k + 1u; j < n; j++) {
				/*
				 * Cross product then exact divide by prev.
				 * Bareiss guarantees divisibility over Z when
				 * intermediates stay within the ring (here int64).
				 */
				num = akk * m[i * n + j] - aik * m[k * n + j];
				m[i * n + j] = num / prev;
			}
			/* Optional: clear below pivot (not needed for det). */
			m[i * n + k] = 0;
		}
	}

	/* det = sign * final diagonal entry. */
	{
		int64_t d = m[(n - 1u) * n + (n - 1u)];

		if (sign < 0) {
			d = -d;
		}
		return d;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mat_det_i64 — integer determinant of an n×n int64 matrix, n <= 8.
 *
 * a is row-major (a[i*n + j] = entry (i,j)). On success writes *out_det
 * and returns 0. Bad arguments return -1 without writing *out_det when
 * out_det is NULL; if out_det is non-NULL but a is NULL / n is bad, still -1.
 * Singular → *out_det = 0, return 0. Empty n==0 → *out_det = 1.
 */
int
gj_mat_det_i64(const int64_t *a, size_t n, int64_t *out_det)
{
	int64_t m[B253_NMAX * B253_NMAX];
	size_t i;
	size_t nn;

	if (out_det == NULL || n > (size_t)B253_NMAX) {
		return -1;
	}
	/* Empty matrix: det = 1; a may be NULL. */
	if (n == 0u) {
		*out_det = 1;
		return 0;
	}
	if (a == NULL) {
		return -1;
	}
	if (n == 1u) {
		*out_det = a[0];
		return 0;
	}

	/* Copy into local stack buffer (freestanding: no malloc). */
	nn = n * n;
	for (i = 0u; i < nn; i++) {
		m[i] = a[i];
	}

	*out_det = b253_bareiss(m, n);
	return 0;
}

int __gj_mat_det_i64(const int64_t *a, size_t n, int64_t *out_det)
    __attribute__((alias("gj_mat_det_i64")));
