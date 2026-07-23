/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2339: freestanding intrusive list unlink
 * (post-2330 list exclusive surface).
 *
 * Surface (unique symbols):
 *   void gj_list_unlink(void **head, void *node);
 *     - Remove node from a doubly-linked intrusive list using its
 *       next/prev links. Layout: word0 = next, word1 = prev.
 *       Updates *head when node was the head. Clears node links.
 *       NULL head or node -> no-op.
 *   void __gj_list_unlink  (alias)
 *   __libcgj_batch2339_marker = "libcgj-batch2339"
 *
 * Post-2330 list exclusive wave (2331-2340). Distinct from
 * gj_list_pop_front/back (batch2335/2336) - unique gj_list_unlink
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2339_marker[] = "libcgj-batch2339";

/* ---- local intrusive node (next, prev) --------------------------------- */

struct b2339_node {
	struct b2339_node *pNext;
	struct b2339_node *pPrev;
};

/* ---- freestanding helpers ---------------------------------------------- */

/* Splice pNode out of the list; clear its links. */
static void
b2339_unlink(void **ppHead, void *pNodeRaw)
{
	struct b2339_node *pNode;
	struct b2339_node *pNext;
	struct b2339_node *pPrev;

	if (ppHead == NULL || pNodeRaw == NULL) {
		return;
	}
	pNode = (struct b2339_node *)pNodeRaw;
	pNext = pNode->pNext;
	pPrev = pNode->pPrev;
	if (pPrev != NULL) {
		pPrev->pNext = pNext;
	} else if (*ppHead == pNode) {
		*ppHead = pNext;
	}
	if (pNext != NULL) {
		pNext->pPrev = pPrev;
	}
	pNode->pNext = NULL;
	pNode->pPrev = NULL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_list_unlink - remove an arbitrary node from an intrusive list.
 *
 * head: address of the list head pointer (NULL -> no-op)
 * node: node to detach (NULL -> no-op); uses its next/prev fields
 *
 * Relinks neighbors in O(1). If node was head, *head advances.
 * Clears next and prev on node. Does not free storage. Does not call libc.
 */
void
gj_list_unlink(void **ppHead, void *pNode)
{
	(void)NULL;
	b2339_unlink(ppHead, pNode);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_list_unlink(void **ppHead, void *pNode)
    __attribute__((alias("gj_list_unlink")));
