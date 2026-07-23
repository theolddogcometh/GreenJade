/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8326: bsearch single-element domain (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_bsearch_one_u_8326(const uint32_t *a, size_t n, uint32_t v);
 *     - Handle the n <= 1 bsearch edge: n == 0 or a == NULL → return n
 *       (not found). n == 1 → return 0 if a[0] == v, else 1 (n).
 *       n > 1 → return n (caller should use full bsearch; this stub
 *       only covers the one-element domain).
 *   size_t __gj_bsearch_one_u_8326  (alias)
 *   __libcgj_batch8326_marker = "libcgj-batch8326"
 *
 * Exclusive continuum CREATE-ONLY (8321-8330: qsort/bsearch compare
 * stubs). Distinct from gj_bsearch_u32 (batch407) — unique _u_8326
 * surface; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8326_marker[] = "libcgj-batch8326";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Single-element (or empty) bsearch. Not-found sentinel is n.
 * Requires n == 1 and non-NULL a for a real probe.
 */
static size_t
b8326_one(const uint32_t *pA, size_t cN, uint32_t u32V)
{
	if (pA == NULL || cN == 0u) {
		return cN;
	}
	if (cN != 1u) {
		return cN;
	}
	if (pA[0] == u32V) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bsearch_one_u_8326 - bsearch for empty / single-element u32 tables.
 *
 * a: sorted table of n uint32_t (may be NULL)
 * n: element count
 * v: key
 *
 * Returns matching index 0 when n == 1 and a[0] == v; else n (absent).
 * Does not binary-search n > 1. No parent wires.
 */
size_t
gj_bsearch_one_u_8326(const uint32_t *pA, size_t cN, uint32_t u32V)
{
	(void)NULL;
	return b8326_one(pA, cN, u32V);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_bsearch_one_u_8326(const uint32_t *pA, size_t cN, uint32_t u32V)
    __attribute__((alias("gj_bsearch_one_u_8326")));
