/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2867: minimum of a uint64_t array (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_arr_u64_min_u(const uint64_t *a, size_t n);
 *     — Return the least value in a[0..n). a == NULL or n == 0 → 0.
 *   uint64_t __gj_arr_u64_min_u  (alias)
 *   __libcgj_batch2867_marker = "libcgj-batch2867"
 *
 * Bitmap/array exclusive wave (2861-2870). Distinct from
 * gj_arr_u32_min_u (batch2864) and pairwise min surfaces — unique
 * gj_arr_u64_min_u value surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2867_marker[] = "libcgj-batch2867";

/* ---- freestanding helpers ---------------------------------------------- */

/* Least value in pA[0..cN). Caller guarantees pA non-NULL and cN > 0. */
static uint64_t
b2867_min(const uint64_t *pA, size_t cN)
{
	size_t i;
	uint64_t u64Min;

	u64Min = pA[0];
	for (i = 1u; i < cN; i++) {
		if (pA[i] < u64Min) {
			u64Min = pA[i];
		}
	}
	return u64Min;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_arr_u64_min_u — least value in a[0..n).
 *
 * a: base (NULL → 0)
 * n: element count (0 → 0)
 *
 * Does not call libc.
 */
uint64_t
gj_arr_u64_min_u(const uint64_t *pA, size_t cN)
{
	(void)NULL;
	if (pA == NULL || cN == 0u) {
		return 0u;
	}
	return b2867_min(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_arr_u64_min_u(const uint64_t *pA, size_t cN)
    __attribute__((alias("gj_arr_u64_min_u")));
