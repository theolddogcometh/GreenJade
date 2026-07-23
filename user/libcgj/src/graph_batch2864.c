/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2864: minimum of a uint32_t array (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_arr_u32_min_u(const uint32_t *a, size_t n);
 *     — Return the least value in a[0..n). a == NULL or n == 0 → 0.
 *   uint32_t __gj_arr_u32_min_u  (alias)
 *   __libcgj_batch2864_marker = "libcgj-batch2864"
 *
 * Bitmap/array exclusive wave (2861-2870). Distinct from gj_u32_min_n
 * (batch2065), gj_min_u32 (batch834), and gj_u32_find_min (batch869) —
 * unique gj_arr_u32_min_u value surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2864_marker[] = "libcgj-batch2864";

/* ---- freestanding helpers ---------------------------------------------- */

/* Least value in pA[0..cN). Caller guarantees pA non-NULL and cN > 0. */
static uint32_t
b2864_min(const uint32_t *pA, size_t cN)
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
 * gj_arr_u32_min_u — least value in a[0..n).
 *
 * a: base (NULL → 0)
 * n: element count (0 → 0)
 *
 * Does not call libc.
 */
uint32_t
gj_arr_u32_min_u(const uint32_t *pA, size_t cN)
{
	(void)NULL;
	if (pA == NULL || cN == 0u) {
		return 0u;
	}
	return b2864_min(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_arr_u32_min_u(const uint32_t *pA, size_t cN)
    __attribute__((alias("gj_arr_u32_min_u")));
