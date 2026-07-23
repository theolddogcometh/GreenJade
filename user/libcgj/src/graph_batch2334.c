/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2334: freestanding intrusive list push back
 * (post-2330 list exclusive surface).
 *
 * Surface (unique symbols):
 *   void gj_list_push_back(void **head, void *node);
 *     - Insert node at the tail of a doubly-linked intrusive list.
 *       Layout: word0 = next, word1 = prev. NULL head or node -> no-op.
 *       Empty list becomes a single-node list.
 *   void __gj_list_push_back  (alias)
 *   __libcgj_batch2334_marker = "libcgj-batch2334"
 *
 * Post-2330 list exclusive wave (2331-2340). Distinct from
 * gj_deque_u32_push_back (batch438) - unique gj_list_push_back
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2334_marker[] = "libcgj-batch2334";

/* ---- local intrusive node (next, prev) --------------------------------- */

struct b2334_node {
	struct b2334_node *pNext;
	struct b2334_node *pPrev;
};

/* ---- freestanding helpers ---------------------------------------------- */

/* Link pNode after the current tail of *ppHead. */
static void
b2334_push_back(void **ppHead, void *pNodeRaw)
{
	struct b2334_node *pNode;
	struct b2334_node *pCur;
	struct b2334_node *pTail;

	if (ppHead == NULL || pNodeRaw == NULL) {
		return;
	}
	pNode = (struct b2334_node *)pNodeRaw;
	pNode->pNext = NULL;
	pCur = (struct b2334_node *)*ppHead;
	if (pCur == NULL) {
		pNode->pPrev = NULL;
		*ppHead = pNode;
		return;
	}
	pTail = pCur;
	while (pTail->pNext != NULL) {
		pTail = pTail->pNext;
	}
	pTail->pNext = pNode;
	pNode->pPrev = pTail;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_list_push_back - insert an intrusive node at the list tail.
 *
 * head: address of the list head pointer (NULL -> no-op)
 * node: node to insert (NULL -> no-op); first two words are next/prev
 *
 * Walks next links to the current tail (O(n)). Does not call libc.
 */
void
gj_list_push_back(void **ppHead, void *pNode)
{
	(void)NULL;
	b2334_push_back(ppHead, pNode);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_list_push_back(void **ppHead, void *pNode)
    __attribute__((alias("gj_list_push_back")));
