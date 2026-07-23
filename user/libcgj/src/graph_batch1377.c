/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1377: saturating int64_t multiply.
 *
 * Surface (unique symbols):
 *   int64_t gj_i64_saturating_mul(int64_t a, int64_t b);
 *     — a * b with saturation at INT64_MAX / INT64_MIN (no wrap).
 *   int64_t __gj_i64_saturating_mul  (alias)
 *   __libcgj_batch1377_marker = "libcgj-batch1377"
 *
 * Unique surface only; no multi-def. Distinct from unsigned sat mul paths.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1377_marker[] = "libcgj-batch1377";

/* ---- freestanding helpers ---------------------------------------------- */

static int64_t
b1377_iabs(int64_t i64V)
{
	/* INT64_MIN is handled by the special-case path in the public API. */
	return (i64V < 0) ? -i64V : i64V;
}

/*
 * Return 0 if a*b is representable in int64_t; +1 if positive overflow;
 * -1 if negative overflow. Assumes neither factor is 0.
 * Special: INT64_MIN * anything nonzero is checked carefully.
 */
static int
b1377_mul_ovf_dir(int64_t i64A, int64_t i64B)
{
	int iNeg;
	uint64_t u64AbsA;
	uint64_t u64AbsB;
	uint64_t u64Lim;

	iNeg = ((i64A < 0) != (i64B < 0)) ? 1 : 0;

	/* Magnitude: careful with INT64_MIN (no positive dual in int64_t). */
	if (i64A == INT64_MIN) {
		u64AbsA = (uint64_t)INT64_MAX + 1ull;
	} else {
		u64AbsA = (uint64_t)b1377_iabs(i64A);
	}
	if (i64B == INT64_MIN) {
		u64AbsB = (uint64_t)INT64_MAX + 1ull;
	} else {
		u64AbsB = (uint64_t)b1377_iabs(i64B);
	}

	if (iNeg) {
		/* Product must be >= INT64_MIN, i.e. magnitude <= 2^63. */
		u64Lim = (uint64_t)INT64_MAX + 1ull;
	} else {
		/* Product must be <= INT64_MAX, i.e. magnitude <= 2^63-1. */
		u64Lim = (uint64_t)INT64_MAX;
	}

	if (u64AbsA > 0ull && u64AbsB > (u64Lim / u64AbsA)) {
		return iNeg ? -1 : 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i64_saturating_mul — signed multiply; clamp to INT64_MAX/MIN on overflow.
 *
 * Zero factors are exact (product 0). Overflow direction follows the sign
 * of the mathematical product.
 */
int64_t
gj_i64_saturating_mul(int64_t i64A, int64_t i64B)
{
	int iDir;

	if (i64A == 0 || i64B == 0) {
		return 0;
	}

	iDir = b1377_mul_ovf_dir(i64A, i64B);
	if (iDir > 0) {
		return INT64_MAX;
	}
	if (iDir < 0) {
		return INT64_MIN;
	}
	return i64A * i64B;
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_i64_saturating_mul(int64_t i64A, int64_t i64B)
    __attribute__((alias("gj_i64_saturating_mul")));
