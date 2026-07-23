/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2423: floor average of a uint32_t array
 * (n-bound exclusive surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_avg_floor_n(const uint32_t *a, size_t n);
 *     - Return floor((a[0] + ... + a[n-1]) / n). NULL a or n == 0
 *       yields 0. Partial sum uses a saturating uint64_t accumulator
 *       so huge n does not wrap before the divide.
 *   uint32_t __gj_u32_avg_floor_n  (alias)
 *   __libcgj_batch2423_marker = "libcgj-batch2423"
 *
 * Array reduce exclusive wave (2421-2430). Distinct from gj_avg_u64
 * (batch381 pairwise) - array avg_floor surface only; no multi-def.
 * Portable sat add only; no __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2423_marker[] = "libcgj-batch2423";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a + b in the uint64_t domain. */
static uint64_t
b2423_sat_add_u64(uint64_t u64A, uint64_t u64B)
{
	if (u64B > (UINT64_MAX - u64A)) {
		return UINT64_MAX;
	}
	return u64A + u64B;
}

/*
 * floor(sum(a[0..cN)) / cN) with sat u64 sum. Caller guarantees pA
 * non-NULL and cN > 0.
 */
static uint32_t
b2423_avg_floor(const uint32_t *pA, size_t cN)
{
	size_t i;
	uint64_t u64Sum;
	uint64_t u64Avg;

	u64Sum = 0ull;
	for (i = 0u; i < cN; i++) {
		u64Sum = b2423_sat_add_u64(u64Sum, (uint64_t)pA[i]);
	}
	u64Avg = u64Sum / (uint64_t)cN;
	if (u64Avg > (uint64_t)UINT32_MAX) {
		return UINT32_MAX;
	}
	return (uint32_t)u64Avg;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_avg_floor_n - floor average of n uint32_t elements.
 *
 * a: base (NULL -> 0)
 * n: element count (0 -> 0)
 *
 * Returns floor(sum / n) as uint32_t (clamped to UINT32_MAX if the
 * saturated sum / n still exceeds 32 bits, which only occurs on full
 * saturation of the 64-bit accumulator).
 */
uint32_t
gj_u32_avg_floor_n(const uint32_t *a, size_t n)
{
	(void)NULL;
	if (n == 0u || a == NULL) {
		return 0u;
	}
	return b2423_avg_floor(a, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_avg_floor_n(const uint32_t *a, size_t n)
    __attribute__((alias("gj_u32_avg_floor_n")));
