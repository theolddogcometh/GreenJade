/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch227: modular linear system solve via
 * Gaussian elimination on an augmented matrix over Z/modZ.
 *
 * Surface (unique symbols):
 *   int gj_solve_linear_mod(int64_t *a, size_t n, int64_t mod);
 *     — a is an n×(n+1) row-major augmented matrix [A | b].
 *       Element (i,j) is a[i*(n+1)+j]; RHS is column n.
 *       On success (return 0) the unique solution x is written into the
 *       RHS column: a[i*(n+1)+n] = x_i, reduced to [0, mod).
 *       Returns -1 on NULL a, mod < 2, singular / non-invertible pivot,
 *       or inconsistent system. n==0 → 0 (empty system).
 *   __gj_solve_linear_mod  (alias)
 *   __libcgj_batch227_marker = "libcgj-batch227"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No malloc, no errno, no __int128. No third-party
 * source copied. Works for general mod >= 2 (not necessarily prime): a
 * pivot must be coprime to mod (modular inverse via extended Euclid).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch227_marker[] = "libcgj-batch227";

/* ---- modular helpers --------------------------------------------------- */

/* Reduce v into [0, m). m > 0. */
static int64_t
b227_norm(int64_t v, int64_t m)
{
	int64_t r = v % m;

	if (r < 0) {
		r += m;
	}
	return r;
}

/*
 * Extended Euclidean: return g = gcd(|x|, |y|) with g >= 0 and
 * *pS * x + *pT * y = g  (coefficients may be negative).
 * Works for any int64 inputs; internal abs is careful about INT64_MIN.
 */
static int64_t
b227_egcd(int64_t x, int64_t y, int64_t *pS, int64_t *pT)
{
	int64_t old_r, r;
	int64_t old_s, s;
	int64_t old_t, t;
	int64_t q;
	int64_t tmp;

	/* Work with non-negative residues for the remainder sequence. */
	if (x < 0) {
		x = -x;
	}
	if (y < 0) {
		y = -y;
	}

	old_r = x;
	r = y;
	old_s = 1;
	s = 0;
	old_t = 0;
	t = 1;

	while (r != 0) {
		q = old_r / r;

		tmp = old_r - q * r;
		old_r = r;
		r = tmp;

		tmp = old_s - q * s;
		old_s = s;
		s = tmp;

		tmp = old_t - q * t;
		old_t = t;
		t = tmp;
	}

	*pS = old_s;
	*pT = old_t;
	return old_r;
}

/*
 * Modular inverse of a mod m. a is reduced first.
 * Returns 0 and writes *pInv when gcd(a,m)==1; else -1.
 */
static int
b227_inv(int64_t a, int64_t m, int64_t *pInv)
{
	int64_t s, t;
	int64_t g;
	int64_t aa;

	aa = b227_norm(a, m);
	if (aa == 0) {
		return -1;
	}
	/*
	 * egcd(aa, m) → s*aa + t*m = g. Want s*aa ≡ 1 (mod m).
	 * Note: b227_egcd takes abs of inputs; aa,m already >= 0.
	 */
	g = b227_egcd(aa, m, &s, &t);
	(void)t;
	if (g != 1) {
		return -1;
	}
	*pInv = b227_norm(s, m);
	return 0;
}

/*
 * (a * b) mod m without 128-bit intermediates.
 * a,b may be any int64; m >= 2. Result in [0, m).
 * Binary modular multiply (same idea as batch180, signed-safe).
 */
static int64_t
b227_mulmod(int64_t a, int64_t b, int64_t m)
{
	int64_t aa;
	int64_t bb;
	int64_t r;

	aa = b227_norm(a, m);
	bb = b227_norm(b, m);
	/* Non-negative factors; result always in [0, m). */
	r = 0;
	while (bb != 0) {
		if ((bb & 1) != 0) {
			/* r = (r + aa) mod m */
			int64_t s = m - r;

			r = (aa >= s) ? (aa - s) : (r + aa);
		}
		{
			/* aa = (aa + aa) mod m */
			int64_t s = m - aa;

			aa = (aa >= s) ? (aa - s) : (aa + aa);
		}
		bb >>= 1;
	}
	return r;
}

/* (a - b) mod m. Result in [0, m). */
static int64_t
b227_submod(int64_t a, int64_t b, int64_t m)
{
	int64_t aa = b227_norm(a, m);
	int64_t bb = b227_norm(b, m);

	if (aa >= bb) {
		return aa - bb;
	}
	return m - (bb - aa);
}

/* ---- row helpers ------------------------------------------------------- */

/* a is n×(n+1); row i starts at a[i * (n+1)]. */
static void
b227_swap_rows(int64_t *a, size_t n, size_t r1, size_t r2)
{
	size_t cols = n + 1u;
	size_t j;
	int64_t tmp;

	if (r1 == r2) {
		return;
	}
	for (j = 0u; j < cols; j++) {
		tmp = a[r1 * cols + j];
		a[r1 * cols + j] = a[r2 * cols + j];
		a[r2 * cols + j] = tmp;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_solve_linear_mod — solve A x ≡ b (mod mod) in place on augmented a.
 *
 * Forward elimination with partial pivoting (first invertible pivot in
 * column col among rows col..n-1). Scale pivot row so diagonal = 1, then
 * eliminate the column in all other rows (Gauss–Jordan). Back-sub is then
 * free: RHS column holds the unique solution.
 *
 * Complexity: O(n^3) modular multiplies. No heap allocation.
 */
int
gj_solve_linear_mod(int64_t *a, size_t n, int64_t mod)
{
	size_t cols;
	size_t col;
	size_t i;
	size_t j;
	size_t pivot;
	int64_t inv;
	int64_t factor;
	int64_t val;

	if (a == NULL) {
		return -1;
	}
	if (mod < 2) {
		return -1;
	}
	if (n == 0u) {
		return 0;
	}

	cols = n + 1u;

	/* Normalize every entry into [0, mod) once. */
	for (i = 0u; i < n; i++) {
		for (j = 0u; j < cols; j++) {
			a[i * cols + j] = b227_norm(a[i * cols + j], mod);
		}
	}

	for (col = 0u; col < n; col++) {
		/* Find invertible pivot in column col, rows col..n-1. */
		pivot = (size_t)-1;
		for (i = col; i < n; i++) {
			val = a[i * cols + col];
			if (val == 0) {
				continue;
			}
			if (b227_inv(val, mod, &inv) == 0) {
				pivot = i;
				break;
			}
		}
		if (pivot == (size_t)-1) {
			/* No invertible pivot: singular over Z/modZ. */
			return -1;
		}

		b227_swap_rows(a, n, col, pivot);

		/* Scale pivot row so a[col][col] == 1. */
		val = a[col * cols + col];
		if (b227_inv(val, mod, &inv) != 0) {
			return -1;
		}
		for (j = col; j < cols; j++) {
			a[col * cols + j] =
			    b227_mulmod(a[col * cols + j], inv, mod);
		}

		/* Eliminate column col in every other row. */
		for (i = 0u; i < n; i++) {
			if (i == col) {
				continue;
			}
			factor = a[i * cols + col];
			if (factor == 0) {
				continue;
			}
			for (j = col; j < cols; j++) {
				/* row_i -= factor * row_col */
				int64_t prod =
				    b227_mulmod(factor, a[col * cols + j],
				                mod);

				a[i * cols + j] =
				    b227_submod(a[i * cols + j], prod, mod);
			}
		}
	}

	/*
	 * Matrix is I | x. Consistency is automatic for square full-rank
	 * systems; RHS already reduced. Solution lives in column n.
	 */
	return 0;
}

int __gj_solve_linear_mod(int64_t *a, size_t n, int64_t mod)
    __attribute__((alias("gj_solve_linear_mod")));
