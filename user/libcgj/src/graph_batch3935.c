/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3935: freestanding index-based singly-linked
 * uint32_t list pop-front (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_slist_u32_pop_u(uint32_t *val, size_t *next, size_t *head,
 *                          size_t *free_head, uint32_t *out);
 *     - Pop the live head slot into *out and return that slot to the
 *       freelist LIFO. Returns 0 on success, -1 if empty or bad args.
 *   int __gj_slist_u32_pop_u  (alias)
 *   __libcgj_batch3935_marker = "libcgj-batch3935"
 *
 * Index layout (caller-owned storage; no malloc):
 *   val[i] / next[i] / *head / *free_head as in batch3934
 * Sentinel: B3935_NIL = ~(size_t)0
 *
 * Exclusive continuum CREATE-ONLY (3931-3940). Distinct from
 * gj_list_pop_front (batch2335), gj_list_u32_remove_u (batch3336), and
 * gj_stack_u32_pop_u (batch2668) — unique slist_u32_pop_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3935_marker[] = "libcgj-batch3935";

/* Terminal / empty list or freelist sentinel. */
#define B3935_NIL (~(size_t)0)

/* ---- freestanding helpers ---------------------------------------------- */

/* Push iSlot onto the freelist LIFO. */
static void
b3935_free(size_t *pNext, size_t *pFreeHead, size_t iSlot)
{
	pNext[iSlot] = *pFreeHead;
	*pFreeHead = iSlot;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_slist_u32_pop_u - pop the front of an index-based slist into *out.
 *
 * Advances *head to next[old_head], copies val[old_head] to *out, and
 * returns the slot to *free_head. Returns 0 on success; -1 if the list
 * is empty or any required pointer is NULL. Cursors and *out are left
 * untouched on failure.
 */
int
gj_slist_u32_pop_u(uint32_t *pVal, size_t *pNext, size_t *pHead,
                   size_t *pFreeHead, uint32_t *pOut)
{
	size_t iHead;
	size_t iNext;

	(void)NULL;

	if (pVal == NULL || pNext == NULL || pHead == NULL ||
	    pFreeHead == NULL || pOut == NULL) {
		return -1;
	}

	iHead = *pHead;
	if (iHead == B3935_NIL) {
		return -1;
	}

	iNext = pNext[iHead];
	*pOut = pVal[iHead];
	*pHead = iNext;
	b3935_free(pNext, pFreeHead, iHead);
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_slist_u32_pop_u(uint32_t *pVal, size_t *pNext, size_t *pHead,
                         size_t *pFreeHead, uint32_t *pOut)
    __attribute__((alias("gj_slist_u32_pop_u")));
