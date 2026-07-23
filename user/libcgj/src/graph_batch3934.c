/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3934: freestanding index-based singly-linked
 * uint32_t list push-front (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_slist_u32_push_u(uint32_t *val, size_t *next, size_t *head,
 *                           size_t *free_head, uint32_t v);
 *     - Allocate one slot from *free_head (LIFO freelist in next[]),
 *       store v in val[slot], and push that slot as the new list head.
 *       Returns 0 on success, -1 if freelist empty or bad args.
 *   int __gj_slist_u32_push_u  (alias)
 *   __libcgj_batch3934_marker = "libcgj-batch3934"
 *
 * Index layout (caller-owned storage; no malloc):
 *   val[i]      — payload at slot i
 *   next[i]     — next live index, or B3934_NIL terminal / free chain
 *   *head       — live list head index (B3934_NIL when empty)
 *   *free_head  — free-slot LIFO head (B3934_NIL when exhausted)
 * Sentinel: B3934_NIL = ~(size_t)0
 *
 * Exclusive continuum CREATE-ONLY (3931-3940). Distinct from
 * gj_list_push_front (batch2333), gj_list_u32_insert_u (batch3335), and
 * gj_stack_u32_push_u (batch2667) — unique slist_u32_push_u surface only;
 * no multi-def. No parent wires (does not call slab_init / list helpers).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3934_marker[] = "libcgj-batch3934";

/* Terminal / empty freelist sentinel. */
#define B3934_NIL (~(size_t)0)

/* ---- freestanding helpers ---------------------------------------------- */

/* Pop one free slot; return its index or B3934_NIL. */
static size_t
b3934_alloc(size_t *pNext, size_t *pFreeHead)
{
	size_t iSlot;

	iSlot = *pFreeHead;
	if (iSlot == B3934_NIL) {
		return B3934_NIL;
	}
	*pFreeHead = pNext[iSlot];
	return iSlot;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_slist_u32_push_u - push v at the front of an index-based slist.
 *
 * val / next: parallel arrays of slots (NULL either -> -1)
 * head:       live list head in/out (NULL -> -1)
 * free_head:  free LIFO head in/out (NULL -> -1)
 * v:          value to store in the newly allocated slot
 *
 * On success: val[slot]=v, next[slot]=old head, *head=slot; returns 0.
 * On failure (empty freelist or NULL args): state unchanged, returns -1.
 * Caller owns freelist chaining (no parent wires).
 */
int
gj_slist_u32_push_u(uint32_t *pVal, size_t *pNext, size_t *pHead,
                    size_t *pFreeHead, uint32_t u32V)
{
	size_t iSlot;
	size_t iOldHead;

	(void)NULL;

	if (pVal == NULL || pNext == NULL || pHead == NULL ||
	    pFreeHead == NULL) {
		return -1;
	}

	iSlot = b3934_alloc(pNext, pFreeHead);
	if (iSlot == B3934_NIL) {
		return -1;
	}

	iOldHead = *pHead;
	pVal[iSlot] = u32V;
	pNext[iSlot] = iOldHead;
	*pHead = iSlot;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_slist_u32_push_u(uint32_t *pVal, size_t *pNext, size_t *pHead,
                          size_t *pFreeHead, uint32_t u32V)
    __attribute__((alias("gj_slist_u32_push_u")));
