/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2422: saturating sum of a uint64_t array
 * (n-bound exclusive surface).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_sum_sat_n(const uint64_t *a, size_t n);
 *     - Return a[0] + ... + a[n-1] with saturation at UINT64_MAX.
 *       NULL a with n > 0 yields 0; n == 0 yields 0.
 *   uint64_t __gj_u64_sum_sat_n  (alias)
 *   __libcgj_batch2422_marker = "libcgj-batch2422"
 *
 * Array reduce exclusive wave (2421-2430). Distinct from gj_u32_sum_sat_n
 * (batch2421) - 64-bit element surface only; no multi-def. Portable sat
 * add only; no __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2422_marker[] = "libcgj-batch2422";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a + b in the uint64_t domain. */
static uint64_t
b2422_sat_add(uint64_t u64A, uint64_t u64B)
{
	if (u64B > (UINT64_MAX - u64A)) {
		return UINT64_MAX;
	}
	return u64A + u64B;
}

/* Saturating sum of a[0..cN). Caller guarantees pA non-NULL when cN > 0. */
static uint64_t
b2422_sum_sat(const uint64_t *pA, size_t cN)
{
	size_t i;
	uint64_t u64Sum;

	u64Sum = 0ull;
	for (i = 0u; i < cN; i++) {
		u64Sum = b2422_sat_add(u64Sum, pA[i]);
		if (u64Sum == UINT64_MAX) {
			return UINT64_MAX;
		}
	}
	return u64Sum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_sum_sat_n - saturating sum of n uint64_t elements.
 *
 * a: base (NULL with n > 0 -> return 0)
 * n: element count
 *
 * Returns the saturated sum of a[0..n) (0 if a is NULL or n is 0).
 */
uint64_t
gj_u64_sum_sat_n(const uint64_t *a, size_t n)
{
	(void)NULL;
	if (n == 0u || a == NULL) {
		return 0ull;
	}
	return b2422_sum_sat(a, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_sum_sat_n(const uint64_t *a, size_t n)
    __attribute__((alias("gj_u64_sum_sat_n")));
