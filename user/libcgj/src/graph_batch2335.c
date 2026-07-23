/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2335: freestanding intrusive list pop front
 * (post-2330 list exclusive surface).
 *
 * Surface (unique symbols):
 *   void *gj_list_pop_front(void **head);
 *     - Remove and return the head node of a doubly-linked intrusive
 *       list. Layout: word0 = next, word1 = prev. Detaches the node
 *       (next=prev=NULL). NULL head or empty list -> NULL.
 *   void *__gj_list_pop_front  (alias)
 *   __libcgj_batch2335_marker = "libcgj-batch2335"
 *
 * Post-2330 list exclusive wave (2331-2340). Distinct from
 * gj_u32_deque_pop_front (batch705) - unique gj_list_pop_front
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2335_marker[] = "libcgj-batch2335";

/* ---- local intrusive node (next, prev) --------------------------------- */

struct b2335_node {
	struct b2335_node *pNext;
	struct b2335_node *pPrev;
};

/* ---- freestanding helpers ---------------------------------------------- */

/* Detach and return the current head, or NULL if empty. */
static void *
b2335_pop_front(void **ppHead)
{
	struct b2335_node *pNode;
	struct b2335_node *pNext;

	if (ppHead == NULL) {
		return NULL;
	}
	pNode = (struct b2335_node *)*ppHead;
	if (pNode == NULL) {
		return NULL;
	}
	pNext = pNode->pNext;
	*ppHead = pNext;
	if (pNext != NULL) {
		pNext->pPrev = NULL;
	}
	pNode->pNext = NULL;
	pNode->pPrev = NULL;
	return pNode;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_list_pop_front - remove the head node from an intrusive list.
 *
 * head: address of the list head pointer (NULL -> NULL)
 *
 * Returns the former head, or NULL if the list was empty. The returned
 * node has next and prev cleared. Does not call libc.
 */
void *
gj_list_pop_front(void **ppHead)
{
	(void)NULL;
	return b2335_pop_front(ppHead);
}

/* ---- underscored alias ------------------------------------------------- */

void *__gj_list_pop_front(void **ppHead)
    __attribute__((alias("gj_list_pop_front")));
