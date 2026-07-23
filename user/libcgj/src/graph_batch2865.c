/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2865: maximum of a uint32_t array (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_arr_u32_max_u(const uint32_t *a, size_t n);
 *     — Return the greatest value in a[0..n). a == NULL or n == 0 → 0.
 *   uint32_t __gj_arr_u32_max_u  (alias)
 *   __libcgj_batch2865_marker = "libcgj-batch2865"
 *
 * Bitmap/array exclusive wave (2861-2870). Distinct from gj_u32_max_n
 * (batch2066), gj_max_u32 (batch835), and gj_u32_find_max (batch868) —
 * unique gj_arr_u32_max_u value surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2865_marker[] = "libcgj-batch2865";

/* ---- freestanding helpers ---------------------------------------------- */

/* Greatest value in pA[0..cN). Caller guarantees pA non-NULL and cN > 0. */
static uint32_t
b2865_max(const uint32_t *pA, size_t cN)
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
 * gj_arr_u32_max_u — greatest value in a[0..n).
 *
 * a: base (NULL → 0)
 * n: element count (0 → 0)
 *
 * Does not call libc.
 */
uint32_t
gj_arr_u32_max_u(const uint32_t *pA, size_t cN)
{
	(void)NULL;
	if (pA == NULL || cN == 0u) {
		return 0u;
	}
	return b2865_max(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_arr_u32_max_u(const uint32_t *pA, size_t cN)
    __attribute__((alias("gj_arr_u32_max_u")));
