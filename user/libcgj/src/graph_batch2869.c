/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2869: linear search in a uint32_t array (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_arr_u32_find_u(const uint32_t *a, size_t n, uint32_t v);
 *     — Return the least index i with a[i] == v, or n if not found
 *       (also when a == NULL or n == 0).
 *   size_t __gj_arr_u32_find_u  (alias)
 *   __libcgj_batch2869_marker = "libcgj-batch2869"
 *
 * Bitmap/array exclusive wave (2861-2870). Distinct from gj_find_u32
 * (batch403) and gj_u32_find (batch611) — unique gj_arr_u32_find_u
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2869_marker[] = "libcgj-batch2869";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * First index of u32V in pA[0..cN), or cN if absent.
 * Caller guarantees pA non-NULL when cN > 0; empty range returns cN.
 */
static size_t
b2869_find(const uint32_t *pA, size_t cN, uint32_t u32V)
{
	size_t i;

	for (i = 0u; i < cN; i++) {
		if (pA[i] == u32V) {
			return i;
		}
	}
	return cN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_arr_u32_find_u — first index of v in a[0..n), or n if absent.
 *
 * a: base (NULL → return n)
 * n: element count
 * v: value to locate
 *
 * Multiple matches → lowest index. Does not call libc.
 */
size_t
gj_arr_u32_find_u(const uint32_t *pA, size_t cN, uint32_t u32V)
{
	(void)NULL;
	if (pA == NULL) {
		return cN;
	}
	return b2869_find(pA, cN, u32V);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_arr_u32_find_u(const uint32_t *pA, size_t cN, uint32_t u32V)
    __attribute__((alias("gj_arr_u32_find_u")));
