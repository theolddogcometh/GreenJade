/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch546: classic intrusive pointer freelist
 * (caller-owned free_list head; each free node stores next at start).
 *
 * Surface (unique symbols):
 *   void *gj_pool_alloc(void **free_list);
 *     — Pop *free_list. The free node stores its next pointer at the
 *       first word of the block. NULL free_list or empty list → NULL.
 *   void gj_pool_free(void **free_list, void *node);
 *     — Push node onto the freelist (LIFO). Writes *free_list into the
 *       first word of node, then sets *free_list = node. NULL free_list
 *       or NULL node is a no-op.
 *   __gj_pool_alloc / __gj_pool_free  (aliases)
 *   __libcgj_batch546_marker = "libcgj-batch546"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Distinct from index freelist
 * (batch325).
 */

#include <stddef.h>

const char __libcgj_batch546_marker[] = "libcgj-batch546";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pool_alloc — pop one free node from a classic pointer freelist.
 *
 * free_list: address of the freelist head pointer. May be NULL.
 *
 * Returns the former head, or NULL if free_list is NULL or the list is
 * empty. On success, *free_list advances to the next free node (read
 * from the first word of the returned block).
 */
void *
gj_pool_alloc(void **free_list)
{
	void *node;

	if (free_list == NULL || *free_list == NULL) {
		return NULL;
	}

	node = *free_list;
	*free_list = *(void **)node;
	return node;
}

/*
 * gj_pool_free — push node onto the freelist (LIFO).
 *
 * free_list: address of the freelist head pointer.
 * node:      block to reclaim; first word becomes the next link.
 *
 * NULL free_list or NULL node is ignored (no-op). Does not validate
 * double-free or alignment (caller owns slab layout).
 */
void
gj_pool_free(void **free_list, void *node)
{
	if (free_list == NULL || node == NULL) {
		return;
	}

	*(void **)node = *free_list;
	*free_list = node;
}

/* ---- underscored aliases ----------------------------------------------- */

void *__gj_pool_alloc(void **free_list)
    __attribute__((alias("gj_pool_alloc")));

void __gj_pool_free(void **free_list, void *node)
    __attribute__((alias("gj_pool_free")));
