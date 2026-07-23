/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2066: freestanding maximum of a uint32_t
 * array (n-bound exclusive surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_max_n(const uint32_t *a, size_t n);
 *     - Return the maximum of a[0..n). a == NULL or n == 0 -> 0.
 *   uint32_t __gj_u32_max_n  (alias)
 *   __libcgj_batch2066_marker = "libcgj-batch2066"
 *
 * Distinct from gj_max_u32 (batch835 pairwise) and gj_u32_find_max
 * (batch868 index) - unique gj_u32_max_n value surface only; no
 * multi-def. Array u32 exclusive wave (2061-2070).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2066_marker[] = "libcgj-batch2066";

/* ---- freestanding helpers ---------------------------------------------- */

/* Greatest value in a[0..cN). Caller guarantees pA non-NULL and cN > 0. */
static uint32_t
b2066_max(const uint32_t *pA, size_t cN)
{
	size_t i;
	uint32_t u32Max;

	u32Max = pA[0];
	for (i = 1u; i < cN; i++) {
		if (pA[i] > u32Max) {
			u32Max = pA[i];
		}
	}
	return u32Max;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_max_n - greatest value in a[0..n).
 *
 * a: base (NULL -> 0)
 * n: element count (0 -> 0)
 */
uint32_t
gj_u32_max_n(const uint32_t *a, size_t n)
{
	(void)NULL;
	if (a == NULL || n == 0u) {
		return 0u;
	}
	return b2066_max(a, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_max_n(const uint32_t *a, size_t n)
    __attribute__((alias("gj_u32_max_n")));
