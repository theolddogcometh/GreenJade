/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2863: modular sum of a uint32_t array (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_arr_u32_sum_u(const uint32_t *a, size_t n);
 *     — Return the sum of a[0..n) as uint32_t (modular wrap on overflow).
 *       a == NULL or n == 0 → 0.
 *   uint32_t __gj_arr_u32_sum_u  (alias)
 *   __libcgj_batch2863_marker = "libcgj-batch2863"
 *
 * Bitmap/array exclusive wave (2861-2870). Distinct from gj_sum_u32
 * (batch404) — unique gj_arr_u32_sum_u name only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2863_marker[] = "libcgj-batch2863";

/* ---- freestanding helpers ---------------------------------------------- */

/* Modular sum of pA[0..cN). Caller guarantees pA non-NULL and cN > 0. */
static uint32_t
b2863_sum(const uint32_t *pA, size_t cN)
{
	size_t i;
	uint32_t u32Sum;

	u32Sum = 0u;
	for (i = 0u; i < cN; i++) {
		u32Sum += pA[i];
	}
	return u32Sum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_arr_u32_sum_u — modular sum of a[0..n).
 *
 * a: base (NULL → 0)
 * n: element count (0 → 0)
 *
 * Overflow past UINT32_MAX wraps (unsigned arithmetic). Does not call libc.
 */
uint32_t
gj_arr_u32_sum_u(const uint32_t *pA, size_t cN)
{
	(void)NULL;
	if (pA == NULL || cN == 0u) {
		return 0u;
	}
	return b2863_sum(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_arr_u32_sum_u(const uint32_t *pA, size_t cN)
    __attribute__((alias("gj_arr_u32_sum_u")));
