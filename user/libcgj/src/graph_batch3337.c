/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3337: linear search in a uint32_t dense list
 * (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   size_t gj_list_u32_find_u(const uint32_t *a, size_t n, uint32_t v);
 *     - Return the least index i with a[i] == v, or n if not found
 *       (also when a == NULL or n == 0).
 *   size_t __gj_list_u32_find_u  (alias)
 *   __libcgj_batch3337_marker = "libcgj-batch3337"
 *
 * Exclusive continuum CREATE-ONLY (3331-3340). Distinct from
 * gj_arr_u32_find_u (batch2869), gj_u32_find (batch611), and
 * gj_list_contains_p (batch2338) - unique gj_list_u32_find_u surface
 * only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3337_marker[] = "libcgj-batch3337";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * First index of u32V in pA[0..cN), or cN if absent.
 * Caller guarantees pA non-NULL when cN > 0; empty range returns cN.
 */
static size_t
b3337_find(const uint32_t *pA, size_t cN, uint32_t u32V)
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
 * gj_list_u32_find_u - first index of v in a[0..n), or n if absent.
 *
 * a: base (NULL -> return n)
 * n: element count
 * v: value to locate
 *
 * Multiple matches -> lowest index. Does not call libc. No parent wires.
 */
size_t
gj_list_u32_find_u(const uint32_t *a, size_t n, uint32_t v)
{
	(void)NULL;
	if (a == NULL) {
		return n;
	}
	return b3337_find(a, n, v);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_list_u32_find_u(const uint32_t *a, size_t n, uint32_t v)
    __attribute__((alias("gj_list_u32_find_u")));
