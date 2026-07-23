/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch325: intrusive freelist over fixed integer
 * slots (caller-owned next[] + free-head).
 *
 * Surface (unique symbols):
 *   void gj_slab_init(int *next, size_t n, int *free_head);
 *     — Chain next[0..n) into a LIFO freelist: next[i] = i+1 for
 *       i in [0, n-1), next[n-1] = -1, *free_head = 0. n == 0 yields
 *       *free_head = -1 (empty). NULL next or free_head is a no-op
 *       except free_head is set to -1 when non-NULL.
 *   int gj_slab_alloc(int *next, int *free_head);
 *     — Pop the free-list head. Returns the slot index, or -1 if empty
 *       / NULL args. Advances *free_head via next[idx].
 *   void gj_slab_free(int *next, int *free_head, int idx);
 *     — Push idx onto the free list (LIFO). Ignores NULL args or
 *       idx < 0. Does not validate double-free or bounds (caller owns
 *       the table size).
 *   __gj_slab_init / __gj_slab_alloc / __gj_slab_free  (aliases)
 *   __libcgj_batch325_marker = "libcgj-batch325"
 *
 * Intrusive layout: free slots store the next free index in next[i];
 * allocated slots leave next[i] undefined until free. Head is -1 when
 * exhausted. Stack-only caller storage — no heap, no errno.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No third-party source
 * copied. Distinct from fixed-slot intern (batch177).
 */

#include <stddef.h>

const char __libcgj_batch325_marker[] = "libcgj-batch325";

/* Empty free-list / terminal next link. */
#define B325_NIL (-1)

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_slab_init — build a free list over next[0 .. n).
 *
 * next:      caller array of at least n ints (may be NULL only if n == 0
 *            is intended; both pointers are still checked).
 * n:         slot count (0 allowed → empty freelist).
 * free_head: out parameter receiving the first free index, or B325_NIL.
 */
void
gj_slab_init(int *pNext, size_t cSlots, int *pFreeHead)
{
	size_t iSlot;

	if (pFreeHead == NULL) {
		return;
	}
	if (pNext == NULL || cSlots == 0u) {
		*pFreeHead = B325_NIL;
		return;
	}

	for (iSlot = 0u; iSlot + 1u < cSlots; iSlot++) {
		pNext[iSlot] = (int)(iSlot + 1u);
	}
	pNext[cSlots - 1u] = B325_NIL;
	*pFreeHead = 0;
}

/*
 * gj_slab_alloc — pop one free slot.
 *
 * Returns the slot index on success, or -1 if the freelist is empty or
 * arguments are NULL.
 */
int
gj_slab_alloc(int *pNext, int *pFreeHead)
{
	int nIdx;

	if (pNext == NULL || pFreeHead == NULL) {
		return B325_NIL;
	}
	nIdx = *pFreeHead;
	if (nIdx < 0) {
		return B325_NIL;
	}
	*pFreeHead = pNext[(size_t)nIdx];
	return nIdx;
}

/*
 * gj_slab_free — push idx back onto the freelist (LIFO).
 *
 * Does not bounds-check idx against the original n; the caller owns the
 * slab capacity. Negative idx and NULL pointers are ignored.
 */
void
gj_slab_free(int *pNext, int *pFreeHead, int nIdx)
{
	if (pNext == NULL || pFreeHead == NULL || nIdx < 0) {
		return;
	}
	pNext[(size_t)nIdx] = *pFreeHead;
	*pFreeHead = nIdx;
}

void __gj_slab_init(int *pNext, size_t cSlots, int *pFreeHead)
    __attribute__((alias("gj_slab_init")));

int __gj_slab_alloc(int *pNext, int *pFreeHead)
    __attribute__((alias("gj_slab_alloc")));

void __gj_slab_free(int *pNext, int *pFreeHead, int nIdx)
    __attribute__((alias("gj_slab_free")));
