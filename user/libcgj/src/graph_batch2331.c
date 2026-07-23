/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2331: freestanding intrusive doubly-linked
 * list node init (post-2330 list exclusive surface).
 *
 * Surface (unique symbols):
 *   void gj_list_node_init(void *node);
 *     - Zero next and prev at the start of an intrusive list node.
 *       Layout: word0 = next, word1 = prev. NULL node -> no-op.
 *   void __gj_list_node_init  (alias)
 *   __libcgj_batch2331_marker = "libcgj-batch2331"
 *
 * Post-2330 list exclusive wave (2331-2340). Distinct from freelist
 * pools (batch546) - unique gj_list_node_init surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2331_marker[] = "libcgj-batch2331";

/* ---- local intrusive node (next, prev) --------------------------------- */

struct b2331_node {
	struct b2331_node *pNext;
	struct b2331_node *pPrev;
};

/* ---- freestanding helpers ---------------------------------------------- */

/* Clear both link fields on an intrusive node. */
static void
b2331_node_init(void *pNodeRaw)
{
	struct b2331_node *pNode;

	if (pNodeRaw == NULL) {
		return;
	}
	pNode = (struct b2331_node *)pNodeRaw;
	pNode->pNext = NULL;
	pNode->pPrev = NULL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_list_node_init - initialize an intrusive list node link pair.
 *
 * node: pointer to a block whose first two words are next and prev.
 *       NULL -> no-op.
 *
 * Sets next = NULL and prev = NULL. Does not call libc.
 */
void
gj_list_node_init(void *pNode)
{
	(void)NULL;
	b2331_node_init(pNode);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_list_node_init(void *pNode)
    __attribute__((alias("gj_list_node_init")));
