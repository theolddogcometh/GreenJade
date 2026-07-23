/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2345: freestanding BST child count helper
 * (post-2340 tree exclusive surface).
 *
 * Surface (unique symbols):
 *   unsigned gj_bst_child_count(const void *node);
 *     - Count non-NULL left/right children of an intrusive binary-tree
 *       node. Layout: word0 = left, word1 = right. Returns 0, 1, or 2.
 *       NULL node -> 0.
 *   unsigned __gj_bst_child_count  (alias)
 *   __libcgj_batch2345_marker = "libcgj-batch2345"
 *
 * Post-2340 tree exclusive wave (2341-2350). Distinct from
 * gj_bst_balance_factor (batch1297) - unique gj_bst_child_count surface
 * only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2345_marker[] = "libcgj-batch2345";

/* ---- local intrusive binary node (left, right) ------------------------- */

struct b2345_node {
	const struct b2345_node *pLeft;
	const struct b2345_node *pRight;
};

/* ---- freestanding helpers ---------------------------------------------- */

/* Count present children; NULL node contributes zero. */
static unsigned
b2345_child_count(const void *pNodeRaw)
{
	const struct b2345_node *pNode;
	unsigned c;

	if (pNodeRaw == NULL) {
		return 0u;
	}
	pNode = (const struct b2345_node *)pNodeRaw;
	c = 0u;
	if (pNode->pLeft != NULL) {
		c = c + 1u;
	}
	if (pNode->pRight != NULL) {
		c = c + 1u;
	}
	return c;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bst_child_count - number of non-NULL children of a BST node.
 *
 * node: pointer to a block whose first two words are left and right.
 *       NULL -> 0.
 *
 * Returns 0, 1, or 2. Does not call libc. Does not recurse.
 */
unsigned
gj_bst_child_count(const void *pNode)
{
	(void)NULL;
	return b2345_child_count(pNode);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_bst_child_count(const void *pNode)
    __attribute__((alias("gj_bst_child_count")));
