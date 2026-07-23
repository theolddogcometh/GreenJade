/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3627: saturating int64_t multiply
 * (exclusive _u2; rename on collision with batch2043 gj_i64_mul_sat_u).
 *
 * Surface (unique symbols):
 *   int64_t gj_i64_mul_sat_u2(int64_t a, int64_t b);
 *     - a * b with saturation at INT64_MAX / INT64_MIN (no wrap, no UB).
 *   int64_t __gj_i64_mul_sat_u2  (alias)
 *   __libcgj_batch3627_marker = "libcgj-batch3627"
 *
 * CREATE-ONLY exclusive continuum wave (3621-3630). Unique
 * gj_i64_mul_sat_u2 surface only; no multi-def. Distinct from
 * gj_i64_mul_sat_u (batch2043) / gj_i64_saturating_mul (batch1377).
 * Magnitude path via uint64_t; no __int128. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3627_marker[] = "libcgj-batch3627";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b3627_mag - absolute magnitude of x as uint64_t.
 * INT64_MIN maps to 1ull << 63 (only representable abs of that value).
 */
static uint64_t
b3627_mag(int64_t i64X)
{
	uint64_t u64X;

	u64X = (uint64_t)i64X;
	if (i64X < 0) {
		return 0u - u64X;
	}
	return u64X;
}

/*
 * b3627_mul_ovf_dir - 0 if a*b fits in int64_t; +1 positive overflow;
 * -1 negative overflow. Assumes neither factor is zero.
 */
static int
b3627_mul_ovf_dir(int64_t i64A, int64_t i64B)
{
	int fNeg;
	uint64_t u64AbsA;
	uint64_t u64AbsB;
	uint64_t u64Lim;

	fNeg = ((i64A < 0) != (i64B < 0)) ? 1 : 0;
	u64AbsA = b3627_mag(i64A);
	u64AbsB = b3627_mag(i64B);

	if (fNeg != 0) {
		/* Product must be >= INT64_MIN, magnitude <= 2^63. */
		u64Lim = (uint64_t)INT64_MAX + 1ull;
	} else {
		/* Product must be <= INT64_MAX, magnitude <= 2^63-1. */
		u64Lim = (uint64_t)INT64_MAX;
	}

	if (u64AbsA > 0ull && u64AbsB > (u64Lim / u64AbsA)) {
		return (fNeg != 0) ? -1 : 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i64_mul_sat_u2 - signed multiply; clamp to INT64_MAX/MIN on overflow.
 *
 * Zero factors are exact (product 0). Overflow direction follows the
 * sign of the mathematical product. Self-contained; no parent wires.
 */
int64_t
gj_i64_mul_sat_u2(int64_t i64A, int64_t i64B)
{
	int nDir;

	(void)NULL;
	if (i64A == 0 || i64B == 0) {
		return 0;
	}

	nDir = b3627_mul_ovf_dir(i64A, i64B);
	if (nDir > 0) {
		return INT64_MAX;
	}
	if (nDir < 0) {
		return INT64_MIN;
	}
	return i64A * i64B;
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_i64_mul_sat_u2(int64_t i64A, int64_t i64B)
    __attribute__((alias("gj_i64_mul_sat_u2")));
