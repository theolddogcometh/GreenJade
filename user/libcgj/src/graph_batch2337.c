/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2337: freestanding intrusive list length
 * (post-2330 list exclusive surface).
 *
 * Surface (unique symbols):
 *   size_t gj_list_len_n(void *const *head);
 *     - Count nodes reachable by walking next from *head.
 *       Layout: word0 = next, word1 = prev. NULL head or empty -> 0.
 *   size_t __gj_list_len_n  (alias)
 *   __libcgj_batch2337_marker = "libcgj-batch2337"
 *
 * Post-2330 list exclusive wave (2331-2340). Distinct from
 * gj_pool_count_free (batch973) - unique gj_list_len_n surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2337_marker[] = "libcgj-batch2337";

/* ---- local intrusive node (next, prev) --------------------------------- */

struct b2337_node {
	struct b2337_node *pNext;
	struct b2337_node *pPrev;
};

/* ---- freestanding helpers ---------------------------------------------- */

/* Count nodes along the next chain. */
static size_t
b2337_len_n(void *const *ppHead)
{
	const struct b2337_node *pCur;
	size_t cNodes;

	if (ppHead == NULL) {
		return 0u;
	}
	pCur = (const struct b2337_node *)*ppHead;
	cNodes = 0u;
	while (pCur != NULL) {
		cNodes = cNodes + 1u;
		pCur = pCur->pNext;
	}
	return cNodes;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_list_len_n - number of nodes in an intrusive doubly-linked list.
 *
 * head: address of the list head pointer (NULL -> 0)
 *
 * Returns the count of nodes reachable via next links. O(n).
 * Does not call libc. Does not detect cycles.
 */
size_t
gj_list_len_n(void *const *ppHead)
{
	(void)NULL;
	return b2337_len_n(ppHead);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_list_len_n(void *const *ppHead)
    __attribute__((alias("gj_list_len_n")));
