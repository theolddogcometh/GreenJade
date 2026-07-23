/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4178: freestanding count of equal uint32_t
 * values in a bounded array.
 *
 * Surface (unique symbols):
 *   size_t gj_u32_arr_count_eq_u(const uint32_t *a, size_t n, uint32_t v);
 *     - Return how many elements of a[0..n) equal v. a == NULL or
 *       n == 0 → 0.
 *   size_t __gj_u32_arr_count_eq_u  (alias)
 *   __libcgj_batch4178_marker = "libcgj-batch4178"
 *
 * Exclusive continuum CREATE-ONLY (4171-4180). Distinct from
 * gj_count_eq_u32 (batch440) — unique gj_u32_arr_count_eq_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4178_marker[] = "libcgj-batch4178";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4178_count_eq(const uint32_t *pA, size_t cN, uint32_t u32V)
{
	size_t i;
	size_t cHit;

	if (pA == NULL || cN == 0u) {
		return 0u;
	}

	cHit = 0u;
	for (i = 0u; i < cN; i++) {
		if (pA[i] == u32V) {
			cHit++;
		}
	}
	return cHit;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_arr_count_eq_u - count elements equal to v.
 *
 * a: base (NULL → return 0)
 * n: element count (0 → return 0)
 * v: value to match
 *
 * Returns the number of matches in a[0..n). Self-contained; no parent
 * wires.
 */
size_t
gj_u32_arr_count_eq_u(const uint32_t *pA, size_t cN, uint32_t u32V)
{
	(void)NULL;
	return b4178_count_eq(pA, cN, u32V);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_u32_arr_count_eq_u(const uint32_t *pA, size_t cN, uint32_t u32V)
    __attribute__((alias("gj_u32_arr_count_eq_u")));
