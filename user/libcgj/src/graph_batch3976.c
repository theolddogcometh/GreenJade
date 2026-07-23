/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3976: index freelist LIFO pop (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_pool_pop_free_u(size_t *next, size_t *free_head);
 *     - Pop one slot from a size_t index freelist (LIFO). Returns the
 *       former head index and advances *free_head = next[head].
 *       Empty list (free_head == (size_t)-1) or NULL next/free_head
 *       → (size_t)-1. Pair with gj_pool_push_free_u (batch3975).
 *   size_t __gj_pool_pop_free_u  (alias)
 *   __libcgj_batch3976_marker = "libcgj-batch3976"
 *
 * Exclusive continuum CREATE-ONLY (3971-3980). Distinct from
 * gj_pool_alloc (batch546 pointer freelist) and gj_slab_alloc
 * (batch325 int freelist) — unique gj_pool_pop_free_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3976_marker[] = "libcgj-batch3976";

#define B3976_EMPTY  ((size_t)-1)

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b3976_pop(size_t *pNext, size_t *pFreeHead)
{
	size_t uIdx;

	if (pNext == NULL || pFreeHead == NULL) {
		return B3976_EMPTY;
	}
	uIdx = *pFreeHead;
	if (uIdx == B3976_EMPTY) {
		return B3976_EMPTY;
	}
	*pFreeHead = pNext[uIdx];
	return uIdx;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pool_pop_free_u - pop one free slot index from a size_t freelist.
 *
 * next:      caller-owned link array
 * free_head: address of freelist head; (size_t)-1 = empty
 *
 * Returns popped index, or (size_t)-1 if empty / NULL args.
 * No parent wires.
 */
size_t
gj_pool_pop_free_u(size_t *pNext, size_t *pFreeHead)
{
	(void)NULL;
	return b3976_pop(pNext, pFreeHead);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_pool_pop_free_u(size_t *pNext, size_t *pFreeHead)
    __attribute__((alias("gj_pool_pop_free_u")));
