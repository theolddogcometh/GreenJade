/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3975: index freelist LIFO push (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pool_push_free_u(size_t *next, size_t *free_head,
 *                                size_t idx);
 *     - Push slot idx onto a size_t index freelist (LIFO). Writes
 *       *free_head into next[idx], then sets *free_head = idx.
 *       Returns 1 on success, 0 if next or free_head is NULL.
 *       Does not bounds-check idx (caller owns the next[] array).
 *       Empty freelist convention: free_head == (size_t)-1.
 *   uint32_t __gj_pool_push_free_u  (alias)
 *   __libcgj_batch3975_marker = "libcgj-batch3975"
 *
 * Exclusive continuum CREATE-ONLY (3971-3980). Distinct from
 * gj_pool_free (batch546 void pointer freelist), gj_pool_free_list_u
 * (batch3344 pointer freelist with status), and gj_slab_free
 * (batch325 int index freelist) — unique gj_pool_push_free_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3975_marker[] = "libcgj-batch3975";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3975_push(size_t *pNext, size_t *pFreeHead, size_t uIdx)
{
	if (pNext == NULL || pFreeHead == NULL) {
		return 0u;
	}
	pNext[uIdx] = *pFreeHead;
	*pFreeHead = uIdx;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pool_push_free_u - push idx onto a size_t index freelist (LIFO).
 *
 * next:      caller-owned link array (next[idx] stores the prior head)
 * free_head: address of freelist head index; (size_t)-1 = empty
 * idx:       slot to reclaim
 *
 * Returns 1 on success, 0 on NULL next/free_head.
 * Does not validate double-free (caller owns layout).
 * No parent wires.
 */
uint32_t
gj_pool_push_free_u(size_t *pNext, size_t *pFreeHead, size_t uIdx)
{
	(void)NULL;
	return b3975_push(pNext, pFreeHead, uIdx);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pool_push_free_u(size_t *pNext, size_t *pFreeHead, size_t uIdx)
    __attribute__((alias("gj_pool_push_free_u")));
