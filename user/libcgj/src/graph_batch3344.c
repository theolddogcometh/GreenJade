/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3344: pool freelist LIFO push (u-suffix).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pool_free_list_u(void **free_list, void *node);
 *     - Push node onto a classic intrusive pointer freelist (LIFO).
 *       Writes *free_list into the first word of node, then sets
 *       *free_list = node. Returns 1 on success, 0 if free_list or
 *       node is NULL. Soft pure-data freelist helper.
 *   uint32_t __gj_pool_free_list_u  (alias)
 *   __libcgj_batch3344_marker = "libcgj-batch3344"
 *
 * Milestone 3350 exclusive continuum CREATE-ONLY (3341-3350). Distinct
 * from gj_pool_free (batch546 void) and gj_pool_alloc (batch546) —
 * unique gj_pool_free_list_u surface only; no multi-def. No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3344_marker[] = "libcgj-batch3344";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3344_push(void **ppHead, void *pNode)
{
	if (ppHead == NULL || pNode == NULL) {
		return 0u;
	}
	*(void **)pNode = *ppHead;
	*ppHead = pNode;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pool_free_list_u - push node onto an intrusive pointer freelist.
 *
 * free_list: address of the freelist head pointer
 * node:      block to reclaim; first word becomes the next link
 *
 * Returns 1 on success, 0 on NULL free_list or NULL node.
 * Does not validate double-free or alignment (caller owns layout).
 * No parent wires.
 */
uint32_t
gj_pool_free_list_u(void **ppFreeList, void *pNode)
{
	(void)NULL;
	return b3344_push(ppFreeList, pNode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pool_free_list_u(void **ppFreeList, void *pNode)
    __attribute__((alias("gj_pool_free_list_u")));
