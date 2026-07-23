/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2336: freestanding intrusive list pop back
 * (post-2330 list exclusive surface).
 *
 * Surface (unique symbols):
 *   void *gj_list_pop_back(void **head);
 *     - Remove and return the tail node of a doubly-linked intrusive
 *       list. Layout: word0 = next, word1 = prev. Detaches the node
 *       (next=prev=NULL). NULL head or empty list -> NULL.
 *   void *__gj_list_pop_back  (alias)
 *   __libcgj_batch2336_marker = "libcgj-batch2336"
 *
 * Post-2330 list exclusive wave (2331-2340). Distinct from
 * gj_list_pop_front (batch2335) - unique gj_list_pop_back surface
 * only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2336_marker[] = "libcgj-batch2336";

/* ---- local intrusive node (next, prev) --------------------------------- */

struct b2336_node {
	struct b2336_node *pNext;
	struct b2336_node *pPrev;
};

/* ---- freestanding helpers ---------------------------------------------- */

/* Detach and return the current tail, or NULL if empty. */
static void *
b2336_pop_back(void **ppHead)
{
	struct b2336_node *pTail;
	struct b2336_node *pPrev;

	if (ppHead == NULL) {
		return NULL;
	}
	pTail = (struct b2336_node *)*ppHead;
	if (pTail == NULL) {
		return NULL;
	}
	while (pTail->pNext != NULL) {
		pTail = pTail->pNext;
	}
	pPrev = pTail->pPrev;
	if (pPrev != NULL) {
		pPrev->pNext = NULL;
	} else {
		*ppHead = NULL;
	}
	pTail->pNext = NULL;
	pTail->pPrev = NULL;
	return pTail;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_list_pop_back - remove the tail node from an intrusive list.
 *
 * head: address of the list head pointer (NULL -> NULL)
 *
 * Returns the former tail, or NULL if the list was empty. Walks next
 * links (O(n)). The returned node has next and prev cleared.
 * Does not call libc.
 */
void *
gj_list_pop_back(void **ppHead)
{
	(void)NULL;
	return b2336_pop_back(ppHead);
}

/* ---- underscored alias ------------------------------------------------- */

void *__gj_list_pop_back(void **ppHead)
    __attribute__((alias("gj_list_pop_back")));
