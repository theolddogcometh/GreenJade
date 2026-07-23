/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2333: freestanding intrusive list push front
 * (post-2330 list exclusive surface).
 *
 * Surface (unique symbols):
 *   void gj_list_push_front(void **head, void *node);
 *     - Insert node at the front of a doubly-linked intrusive list.
 *       Layout: word0 = next, word1 = prev. NULL head or node -> no-op.
 *   void __gj_list_push_front  (alias)
 *   __libcgj_batch2333_marker = "libcgj-batch2333"
 *
 * Post-2330 list exclusive wave (2331-2340). Distinct from
 * gj_deque_u32_push_front (batch438) - unique gj_list_push_front
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2333_marker[] = "libcgj-batch2333";

/* ---- local intrusive node (next, prev) --------------------------------- */

struct b2333_node {
	struct b2333_node *pNext;
	struct b2333_node *pPrev;
};

/* ---- freestanding helpers ---------------------------------------------- */

/* Link pNode as the new head of *ppHead. */
static void
b2333_push_front(void **ppHead, void *pNodeRaw)
{
	struct b2333_node *pNode;
	struct b2333_node *pOld;

	if (ppHead == NULL || pNodeRaw == NULL) {
		return;
	}
	pNode = (struct b2333_node *)pNodeRaw;
	pOld = (struct b2333_node *)*ppHead;
	pNode->pNext = pOld;
	pNode->pPrev = NULL;
	if (pOld != NULL) {
		pOld->pPrev = pNode;
	}
	*ppHead = pNode;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_list_push_front - insert an intrusive node at the list head.
 *
 * head: address of the list head pointer (NULL -> no-op)
 * node: node to insert (NULL -> no-op); first two words are next/prev
 *
 * On success *head becomes node. Does not call libc.
 */
void
gj_list_push_front(void **ppHead, void *pNode)
{
	(void)NULL;
	b2333_push_front(ppHead, pNode);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_list_push_front(void **ppHead, void *pNode)
    __attribute__((alias("gj_list_push_front")));
