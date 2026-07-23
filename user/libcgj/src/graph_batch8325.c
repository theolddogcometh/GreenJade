/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8325: bsearch empty-domain predicate (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bsearch_empty_u_8325(const void *base, size_t nmemb);
 *     - Return 1 if the search domain is empty (base == NULL or
 *       nmemb == 0); else 0. Soft bsearch precondition helper.
 *   uint32_t __gj_bsearch_empty_u_8325  (alias)
 *   __libcgj_batch8325_marker = "libcgj-batch8325"
 *
 * Exclusive continuum CREATE-ONLY (8321-8330: qsort/bsearch compare
 * stubs). Unique gj_bsearch_empty_u_8325 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8325_marker[] = "libcgj-batch8325";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff domain is empty (NULL base or zero count). */
static uint32_t
b8325_empty(const void *pBase, size_t cN)
{
	if (pBase == NULL) {
		return 1u;
	}
	if (cN == 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bsearch_empty_u_8325 - 1 if bsearch domain is empty.
 *
 * base:  candidate table base (may be NULL)
 * nmemb: element count
 *
 * Returns 1 when search is vacuous (no elements / no base), else 0.
 * Does not perform a key lookup. No parent wires.
 */
uint32_t
gj_bsearch_empty_u_8325(const void *pBase, size_t cN)
{
	(void)NULL;
	return b8325_empty(pBase, cN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bsearch_empty_u_8325(const void *pBase, size_t cN)
    __attribute__((alias("gj_bsearch_empty_u_8325")));
