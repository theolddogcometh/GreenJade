/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch973: count free nodes on a pointer freelist.
 *
 * Surface (unique symbols):
 *   size_t gj_pool_count_free(void *const *free_list);
 *     — Walk a classic intrusive pointer freelist (next stored at the
 *       first word of each free node) and return the node count.
 *       free_list == NULL or empty head → 0. Does not mutate the list.
 *   size_t __gj_pool_count_free  (alias)
 *   __libcgj_batch973_marker = "libcgj-batch973"
 *
 * Distinct from gj_pool_alloc / gj_pool_free (batch546). Unique
 * gj_pool_count_free surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Caller owns freelist integrity
 * (no cycle detection).
 */

#include <stddef.h>

const char __libcgj_batch973_marker[] = "libcgj-batch973";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pool_count_free — count nodes on a classic pointer freelist.
 *
 * free_list: address of the freelist head pointer (may be NULL).
 *
 * Returns the number of free nodes reachable from *free_list.
 * Does not allocate, free, or reorder nodes.
 */
size_t
gj_pool_count_free(void *const *free_list)
{
	const void *pNode;
	size_t cFree;

	if (free_list == NULL) {
		return 0u;
	}

	pNode = *free_list;
	cFree = 0u;
	while (pNode != NULL) {
		cFree++;
		/* Next link lives at the first word of the free node. */
		pNode = *(const void *const *)pNode;
	}
	return cFree;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_pool_count_free(void *const *free_list)
    __attribute__((alias("gj_pool_count_free")));
