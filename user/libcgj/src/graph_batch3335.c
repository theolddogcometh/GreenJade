/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3335: insert one uint32_t into a capacity-
 * bounded list array (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   size_t gj_list_u32_insert_u(uint32_t *a, size_t n, size_t cap,
 *       size_t i, uint32_t v);
 *     - Insert v at index i of a[0..n), shifting a[i..n) one slot right.
 *       Requires room for one more element (n < cap) and i <= n.
 *       Returns the new length (n+1) on success, or the old n on failure
 *       (NULL a, n >= cap, or i > n). On failure the array is unchanged.
 *   size_t __gj_list_u32_insert_u  (alias)
 *   __libcgj_batch3335_marker = "libcgj-batch3335"
 *
 * Exclusive continuum CREATE-ONLY (3331-3340). Distinct from
 * gj_u32_insert_at (batch1007) and gj_list_push_back (batch2334) -
 * unique gj_list_u32_insert_u surface only; no multi-def. No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3335_marker[] = "libcgj-batch3335";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Insert u32V at index iIdx; return new length or old cN on failure.
 * Caller-owned dense list: a[0..cN) live, capacity cCap.
 */
static size_t
b3335_insert(uint32_t *pA, size_t cN, size_t cCap, size_t iIdx,
    uint32_t u32V)
{
	size_t j;

	if (pA == NULL || cN >= cCap || iIdx > cN) {
		return cN;
	}

	/* Shift a[i..n) one slot to the right (walk high -> low). */
	for (j = cN; j > iIdx; j--) {
		pA[j] = pA[j - 1u];
	}
	pA[iIdx] = u32V;
	return cN + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_list_u32_insert_u - insert v at index i; return new length or old n.
 *
 * a:   list base (NULL -> return n)
 * n:   current element count
 * cap: allocated capacity in elements (must be > n to succeed)
 * i:   insertion index in 0..n inclusive (i == n appends)
 * v:   value to insert
 *
 * Shifts a[i..n) right by one, writes a[i] = v, returns n+1. Failures
 * leave a[] unchanged and return n. No parent wires. Does not call libc.
 */
size_t
gj_list_u32_insert_u(uint32_t *a, size_t n, size_t cap, size_t i,
    uint32_t v)
{
	(void)NULL;
	return b3335_insert(a, n, cap, i, v);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_list_u32_insert_u(uint32_t *a, size_t n, size_t cap, size_t i,
    uint32_t v)
    __attribute__((alias("gj_list_u32_insert_u")));
