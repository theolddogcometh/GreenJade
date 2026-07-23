/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2338: freestanding intrusive list contains
 * predicate (post-2330 list exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_list_contains_p(void *const *head, const void *node);
 *     - Return 1 if node is reachable by walking next from *head,
 *       else 0. Layout: word0 = next, word1 = prev.
 *       NULL head, empty list, or NULL node -> 0.
 *   int __gj_list_contains_p  (alias)
 *   __libcgj_batch2338_marker = "libcgj-batch2338"
 *
 * Post-2330 list exclusive wave (2331-2340). Distinct from
 * gj_list_empty_p (batch2332) - unique gj_list_contains_p surface
 * only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2338_marker[] = "libcgj-batch2338";

/* ---- local intrusive node (next, prev) --------------------------------- */

struct b2338_node {
	struct b2338_node *pNext;
	struct b2338_node *pPrev;
};

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if pNode appears on the next-chain from *ppHead. */
static int
b2338_contains_p(void *const *ppHead, const void *pNode)
{
	const struct b2338_node *pCur;
	const struct b2338_node *pWant;

	if (ppHead == NULL || pNode == NULL) {
		return 0;
	}
	pWant = (const struct b2338_node *)pNode;
	pCur = (const struct b2338_node *)*ppHead;
	while (pCur != NULL) {
		if (pCur == pWant) {
			return 1;
		}
		pCur = pCur->pNext;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_list_contains_p - true when node is a member of the list.
 *
 * head: address of the list head pointer (NULL -> 0)
 * node: candidate node pointer (NULL -> 0)
 *
 * Returns 1 if node is found by walking next from *head, else 0.
 * O(n). Does not call libc. Does not detect cycles.
 */
int
gj_list_contains_p(void *const *ppHead, const void *pNode)
{
	(void)NULL;
	return b2338_contains_p(ppHead, pNode);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_list_contains_p(void *const *ppHead, const void *pNode)
    __attribute__((alias("gj_list_contains_p")));
