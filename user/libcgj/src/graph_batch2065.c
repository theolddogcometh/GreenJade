/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2065: freestanding minimum of a uint32_t
 * array (n-bound exclusive surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_min_n(const uint32_t *a, size_t n);
 *     - Return the minimum of a[0..n). a == NULL or n == 0 -> 0.
 *   uint32_t __gj_u32_min_n  (alias)
 *   __libcgj_batch2065_marker = "libcgj-batch2065"
 *
 * Distinct from gj_min_u32 (batch834 pairwise) and gj_u32_find_min
 * (batch869 index) - unique gj_u32_min_n value surface only; no
 * multi-def. Array u32 exclusive wave (2061-2070).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2065_marker[] = "libcgj-batch2065";

/* ---- freestanding helpers ---------------------------------------------- */

/* Least value in a[0..cN). Caller guarantees pA non-NULL and cN > 0. */
static uint32_t
b2065_min(const uint32_t *pA, size_t cN)
{
	size_t i;
	uint32_t u32Min;

	u32Min = pA[0];
	for (i = 1u; i < cN; i++) {
		if (pA[i] < u32Min) {
			u32Min = pA[i];
		}
	}
	return u32Min;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_min_n - least value in a[0..n).
 *
 * a: base (NULL -> 0)
 * n: element count (0 -> 0)
 */
uint32_t
gj_u32_min_n(const uint32_t *a, size_t n)
{
	(void)NULL;
	if (a == NULL || n == 0u) {
		return 0u;
	}
	return b2065_min(a, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_min_n(const uint32_t *a, size_t n)
    __attribute__((alias("gj_u32_min_n")));
