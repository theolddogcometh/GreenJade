/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3937: freestanding index-based doubly-linked
 * uint32_t list insert (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_dlist_u32_insert_u(uint32_t *val, size_t *next, size_t *prev,
 *                             size_t *head, size_t *free_head, size_t pred,
 *                             uint32_t v);
 *     - Allocate one free slot, store v, and insert it after pred in the
 *       live dlist. pred == B3937_NIL inserts at the head. Returns 0 on
 *       success, -1 if freelist empty or bad args.
 *   int __gj_dlist_u32_insert_u  (alias)
 *   __libcgj_batch3937_marker = "libcgj-batch3937"
 *
 * Index layout (caller-owned storage; no malloc):
 *   val[i] / next[i] / prev[i] — payload and bidirectional links
 *   *head / *free_head         — live head and freelist LIFO
 * Sentinel: B3937_NIL = ~(size_t)0
 *
 * Exclusive continuum CREATE-ONLY (3931-3940). Distinct from
 * gj_list_u32_insert_u (batch3335), gj_list_push_front (batch2333), and
 * gj_list_push_back (batch2334) — unique dlist_u32_insert_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3937_marker[] = "libcgj-batch3937";

/* Terminal / empty freelist / insert-at-head sentinel. */
#define B3937_NIL (~(size_t)0)

/* ---- freestanding helpers ---------------------------------------------- */

/* Pop one free slot; return its index or B3937_NIL. */
static size_t
b3937_alloc(size_t *pNext, size_t *pFreeHead)
{
	size_t iSlot;

	iSlot = *pFreeHead;
	if (iSlot == B3937_NIL) {
		return B3937_NIL;
	}
	*pFreeHead = pNext[iSlot];
	return iSlot;
}

/*
 * Insert iSlot after iPred (or at head when iPred is NIL). Updates next,
 * prev, and *pHead. pVal[iSlot] must already hold the payload.
 */
static void
b3937_link_after(size_t *pNext, size_t *pPrev, size_t *pHead, size_t iPred,
                 size_t iSlot)
{
	size_t iSucc;

	if (iPred == B3937_NIL) {
		/* Push front. */
		iSucc = *pHead;
		pNext[iSlot] = iSucc;
		pPrev[iSlot] = B3937_NIL;
		if (iSucc != B3937_NIL) {
			pPrev[iSucc] = iSlot;
		}
		*pHead = iSlot;
		return;
	}

	iSucc = pNext[iPred];
	pNext[iSlot] = iSucc;
	pPrev[iSlot] = iPred;
	pNext[iPred] = iSlot;
	if (iSucc != B3937_NIL) {
		pPrev[iSucc] = iSlot;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dlist_u32_insert_u - insert v after pred in an index-based dlist.
 *
 * val / next / prev: parallel slot arrays (any NULL -> -1)
 * head:              live list head in/out (NULL -> -1)
 * free_head:         free LIFO head in/out (NULL -> -1)
 * pred:              predecessor slot, or B3937_NIL to insert at head
 * v:                 value to store
 *
 * On success returns 0. On freelist empty / NULL args returns -1 with
 * live list and freelist left as before the failed alloc.
 */
int
gj_dlist_u32_insert_u(uint32_t *pVal, size_t *pNext, size_t *pPrev,
                      size_t *pHead, size_t *pFreeHead, size_t iPred,
                      uint32_t u32V)
{
	size_t iSlot;

	(void)NULL;

	if (pVal == NULL || pNext == NULL || pPrev == NULL || pHead == NULL ||
	    pFreeHead == NULL) {
		return -1;
	}

	iSlot = b3937_alloc(pNext, pFreeHead);
	if (iSlot == B3937_NIL) {
		return -1;
	}

	pVal[iSlot] = u32V;
	b3937_link_after(pNext, pPrev, pHead, iPred, iSlot);
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_dlist_u32_insert_u(uint32_t *pVal, size_t *pNext, size_t *pPrev,
                            size_t *pHead, size_t *pFreeHead, size_t iPred,
                            uint32_t u32V)
    __attribute__((alias("gj_dlist_u32_insert_u")));
