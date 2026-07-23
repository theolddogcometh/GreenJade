/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2425: integer variance hint for a uint32_t
 * array — floor(sum of squares / n) (n-bound exclusive surface).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u32_variance_hint_n(const uint32_t *a, size_t n);
 *     - Return floor(sum_i a[i]^2 / n) as a cheap integer second-moment
 *       / variance-scale hint (not a centered variance). NULL a or
 *       n == 0 yields 0. Squares widen u32*u32 into u64 exactly;
 *       sum of squares uses portable sat add.
 *   uint64_t __gj_u32_variance_hint_n  (alias)
 *   __libcgj_batch2425_marker = "libcgj-batch2425"
 *
 * Array reduce exclusive wave (2421-2430). Distinct from Welford
 * surfaces (batch309 / batch896-897) - simple sum-sq/n hint only; no
 * multi-def. Portable sat mul/add only; no __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2425_marker[] = "libcgj-batch2425";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a + b in the uint64_t domain. */
static uint64_t
b2425_sat_add_u64(uint64_t u64A, uint64_t u64B)
{
	if (u64B > (UINT64_MAX - u64A)) {
		return UINT64_MAX;
	}
	return u64A + u64B;
}

/*
 * u32 * u32 always fits in u64 (max product (2^32-1)^2 < 2^64).
 * Kept as a named helper so the wave's mul path is uniform.
 */
static uint64_t
b2425_sq_u32(uint32_t u32V)
{
	return (uint64_t)u32V * (uint64_t)u32V;
}

/*
 * floor(sum(a[i]^2) / cN). Caller guarantees pA non-NULL and cN > 0.
 */
static uint64_t
b2425_var_hint(const uint32_t *pA, size_t cN)
{
	size_t i;
	uint64_t u64SumSq;

	u64SumSq = 0ull;
	for (i = 0u; i < cN; i++) {
		u64SumSq = b2425_sat_add_u64(u64SumSq, b2425_sq_u32(pA[i]));
	}
	return u64SumSq / (uint64_t)cN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_variance_hint_n - floor(sum of squares / n) integer hint.
 *
 * a: base (NULL -> 0)
 * n: element count (0 -> 0)
 *
 * Not a de-meaned sample variance: just the raw second-moment scale
 * sum(x^2)/n. Useful as a cheap energy / magnitude proxy.
 */
uint64_t
gj_u32_variance_hint_n(const uint32_t *a, size_t n)
{
	(void)NULL;
	if (n == 0u || a == NULL) {
		return 0ull;
	}
	return b2425_var_hint(a, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u32_variance_hint_n(const uint32_t *a, size_t n)
    __attribute__((alias("gj_u32_variance_hint_n")));
