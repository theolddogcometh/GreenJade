/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2346: freestanding BST leaf predicate
 * (post-2340 tree exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_bst_is_leaf_p(const void *node);
 *     - Return 1 if node is a leaf (both left and right are NULL).
 *       Layout: word0 = left, word1 = right. NULL node -> 0.
 *   int __gj_bst_is_leaf_p  (alias)
 *   __libcgj_batch2346_marker = "libcgj-batch2346"
 *
 * Post-2340 tree exclusive wave (2341-2350). Distinct from
 * gj_bst_child_count (batch2345) - unique gj_bst_is_leaf_p surface
 * only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2346_marker[] = "libcgj-batch2346";

/* ---- local intrusive binary node (left, right) ------------------------- */

struct b2346_node {
	const struct b2346_node *pLeft;
	const struct b2346_node *pRight;
};

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 when node exists and has no children. */
static int
b2346_is_leaf_p(const void *pNodeRaw)
{
	const struct b2346_node *pNode;

	if (pNodeRaw == NULL) {
		return 0;
	}
	pNode = (const struct b2346_node *)pNodeRaw;
	if (pNode->pLeft != NULL) {
		return 0;
	}
	if (pNode->pRight != NULL) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bst_is_leaf_p - true when a BST node has no children.
 *
 * node: pointer to a block whose first two words are left and right.
 *       NULL -> 0 (not a leaf node object).
 *
 * Returns 1 when both children are NULL, else 0. Does not call libc.
 */
int
gj_bst_is_leaf_p(const void *pNode)
{
	(void)NULL;
	return b2346_is_leaf_p(pNode);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bst_is_leaf_p(const void *pNode)
    __attribute__((alias("gj_bst_is_leaf_p")));
