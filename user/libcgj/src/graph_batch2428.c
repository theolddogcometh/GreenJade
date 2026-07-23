/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2428: uint32_t L1 (Manhattan) distance with
 * saturating accumulation (n-bound exclusive surface).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u32_l1_dist_n(const uint32_t *a, const uint32_t *b,
 *                             size_t n);
 *     - Return sum_i |a[i] - b[i]| as uint64_t with portable sat add.
 *       NULL a or NULL b or n == 0 yields 0.
 *   uint64_t __gj_u32_l1_dist_n  (alias)
 *   __libcgj_batch2428_marker = "libcgj-batch2428"
 *
 * Array reduce exclusive wave (2421-2430). Unique u32 L1 surface only;
 * no multi-def. Portable sat add only; no __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2428_marker[] = "libcgj-batch2428";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a + b in the uint64_t domain. */
static uint64_t
b2428_sat_add_u64(uint64_t u64A, uint64_t u64B)
{
	if (u64B > (UINT64_MAX - u64A)) {
		return UINT64_MAX;
	}
	return u64A + u64B;
}

/* Unsigned absolute difference of two uint32 values (exact in u32). */
static uint32_t
b2428_absdiff_u32(uint32_t u32A, uint32_t u32B)
{
	if (u32A >= u32B) {
		return u32A - u32B;
	}
	return u32B - u32A;
}

/*
 * Saturating L1 of a vs b over cN elements. Caller guarantees both
 * pointers non-NULL when cN > 0.
 */
static uint64_t
b2428_l1(const uint32_t *pA, const uint32_t *pB, size_t cN)
{
	size_t i;
	uint64_t u64Acc;

	u64Acc = 0ull;
	for (i = 0u; i < cN; i++) {
		u64Acc = b2428_sat_add_u64(u64Acc,
		    (uint64_t)b2428_absdiff_u32(pA[i], pB[i]));
		if (u64Acc == UINT64_MAX) {
			return UINT64_MAX;
		}
	}
	return u64Acc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_l1_dist_n - L1 (Manhattan) distance between two uint32 vectors.
 *
 * a, b: length-n vectors (NULL either -> 0)
 * n:    element count (0 -> 0)
 */
uint64_t
gj_u32_l1_dist_n(const uint32_t *a, const uint32_t *b, size_t n)
{
	(void)NULL;
	if (n == 0u || a == NULL || b == NULL) {
		return 0ull;
	}
	return b2428_l1(a, b, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u32_l1_dist_n(const uint32_t *a, const uint32_t *b, size_t n)
    __attribute__((alias("gj_u32_l1_dist_n")));
