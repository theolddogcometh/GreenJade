/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2429: uint32_t squared L2 distance with
 * saturating accumulation (n-bound exclusive surface).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u32_l2sq_dist_n(const uint32_t *a, const uint32_t *b,
 *                               size_t n);
 *     - Return sum_i (a[i] - b[i])^2 as uint64_t. Abs-diff is u32;
 *       square widens to u64 exactly; running sum uses portable sat
 *       add. NULL a or NULL b or n == 0 yields 0.
 *   uint64_t __gj_u32_l2sq_dist_n  (alias)
 *   __libcgj_batch2429_marker = "libcgj-batch2429"
 *
 * Array reduce exclusive wave (2421-2430). Distinct from gj_l2sq_i32
 * (batch540 signed) - unsigned u32_l2sq_dist_n surface only; no
 * multi-def. Portable sat mul/add only; no __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2429_marker[] = "libcgj-batch2429";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a + b in the uint64_t domain. */
static uint64_t
b2429_sat_add_u64(uint64_t u64A, uint64_t u64B)
{
	if (u64B > (UINT64_MAX - u64A)) {
		return UINT64_MAX;
	}
	return u64A + u64B;
}

/* Unsigned absolute difference of two uint32 values. */
static uint32_t
b2429_absdiff_u32(uint32_t u32A, uint32_t u32B)
{
	if (u32A >= u32B) {
		return u32A - u32B;
	}
	return u32B - u32A;
}

/* Exact square of a u32 abs-diff into u64 (always fits). */
static uint64_t
b2429_sq_u32(uint32_t u32D)
{
	return (uint64_t)u32D * (uint64_t)u32D;
}

/*
 * Saturating L2-squared of a vs b over cN elements. Caller guarantees
 * both pointers non-NULL when cN > 0.
 */
static uint64_t
b2429_l2sq(const uint32_t *pA, const uint32_t *pB, size_t cN)
{
	size_t i;
	uint64_t u64Acc;

	u64Acc = 0ull;
	for (i = 0u; i < cN; i++) {
		u64Acc = b2429_sat_add_u64(u64Acc,
		    b2429_sq_u32(b2429_absdiff_u32(pA[i], pB[i])));
		if (u64Acc == UINT64_MAX) {
			return UINT64_MAX;
		}
	}
	return u64Acc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_l2sq_dist_n - squared Euclidean distance of two uint32 vectors.
 *
 * a, b: length-n vectors (NULL either -> 0)
 * n:    element count (0 -> 0)
 */
uint64_t
gj_u32_l2sq_dist_n(const uint32_t *a, const uint32_t *b, size_t n)
{
	(void)NULL;
	if (n == 0u || a == NULL || b == NULL) {
		return 0ull;
	}
	return b2429_l2sq(a, b, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u32_l2sq_dist_n(const uint32_t *a, const uint32_t *b, size_t n)
    __attribute__((alias("gj_u32_l2sq_dist_n")));
