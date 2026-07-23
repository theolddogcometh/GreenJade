/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2421: saturating sum of a uint32_t array
 * (n-bound exclusive surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_sum_sat_n(const uint32_t *a, size_t n);
 *     - Return a[0] + ... + a[n-1] with saturation at UINT32_MAX.
 *       NULL a with n > 0 yields 0; n == 0 yields 0.
 *   uint32_t __gj_u32_sum_sat_n  (alias)
 *   __libcgj_batch2421_marker = "libcgj-batch2421"
 *
 * Array reduce exclusive wave (2421-2430). Distinct from gj_u32_sum_n
 * (batch2061, non-saturating u64 acc) - unique sum_sat surface only;
 * no multi-def. Portable sat add only; no __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2421_marker[] = "libcgj-batch2421";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a + b in the uint32_t domain. */
static uint32_t
b2421_sat_add(uint32_t u32A, uint32_t u32B)
{
	if (u32B > (UINT32_MAX - u32A)) {
		return UINT32_MAX;
	}
	return u32A + u32B;
}

/* Saturating sum of a[0..cN). Caller guarantees pA non-NULL when cN > 0. */
static uint32_t
b2421_sum_sat(const uint32_t *pA, size_t cN)
{
	size_t i;
	uint32_t u32Sum;

	u32Sum = 0u;
	for (i = 0u; i < cN; i++) {
		u32Sum = b2421_sat_add(u32Sum, pA[i]);
		if (u32Sum == UINT32_MAX) {
			/* Early out: further adds stay saturated. */
			return UINT32_MAX;
		}
	}
	return u32Sum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_sum_sat_n - saturating sum of n uint32_t elements.
 *
 * a: base (NULL with n > 0 -> return 0)
 * n: element count
 *
 * Returns the saturated sum of a[0..n) (0 if a is NULL or n is 0).
 */
uint32_t
gj_u32_sum_sat_n(const uint32_t *a, size_t n)
{
	(void)NULL;
	if (n == 0u || a == NULL) {
		return 0u;
	}
	return b2421_sum_sat(a, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_sum_sat_n(const uint32_t *a, size_t n)
    __attribute__((alias("gj_u32_sum_sat_n")));
