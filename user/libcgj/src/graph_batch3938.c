/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3938: freestanding index-based doubly-linked
 * uint32_t list remove (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_dlist_u32_remove_u(uint32_t *val, size_t *next, size_t *prev,
 *                             size_t *head, size_t *free_head, size_t idx,
 *                             uint32_t *out);
 *     - Unlink slot idx from the live dlist, copy val[idx] to *out when
 *       out is non-NULL, and return the slot to the freelist LIFO.
 *       Returns 0 on success, -1 if idx is NIL or required args are bad.
 *   int __gj_dlist_u32_remove_u  (alias)
 *   __libcgj_batch3938_marker = "libcgj-batch3938"
 *
 * Index layout (caller-owned storage; no malloc):
 *   val[i] / next[i] / prev[i] / *head / *free_head as in batch3937
 * Sentinel: B3938_NIL = ~(size_t)0
 *
 * Exclusive continuum CREATE-ONLY (3931-3940). Distinct from
 * gj_list_unlink (batch2339), gj_list_u32_remove_u (batch3336), and
 * gj_slist_u32_pop_u (batch3935) — unique dlist_u32_remove_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3938_marker[] = "libcgj-batch3938";

/* Terminal / invalid-index sentinel. */
#define B3938_NIL (~(size_t)0)

/* ---- freestanding helpers ---------------------------------------------- */

/* Splice iIdx out of the dlist; clear its links; fix *pHead if needed. */
static void
b3938_unlink(size_t *pNext, size_t *pPrev, size_t *pHead, size_t iIdx)
{
	size_t iN;
	size_t iP;

	iN = pNext[iIdx];
	iP = pPrev[iIdx];

	if (iP != B3938_NIL) {
		pNext[iP] = iN;
	} else if (*pHead == iIdx) {
		*pHead = iN;
	}

	if (iN != B3938_NIL) {
		pPrev[iN] = iP;
	}

	pNext[iIdx] = B3938_NIL;
	pPrev[iIdx] = B3938_NIL;
}

/* Push iSlot onto the freelist LIFO. */
static void
b3938_free(size_t *pNext, size_t *pFreeHead, size_t iSlot)
{
	pNext[iSlot] = *pFreeHead;
	*pFreeHead = iSlot;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dlist_u32_remove_u - remove slot idx from an index-based dlist.
 *
 * val / next / prev: parallel slot arrays (val may be NULL only if out
 *                    is also NULL; next/prev/head/free_head required)
 * head / free_head:  live head and freelist LIFO (NULL either -> -1)
 * idx:               slot to unlink (B3938_NIL -> -1)
 * out:               optional; when non-NULL receives val[idx]
 *
 * Relinks neighbors in O(1). Returns 0 on success, -1 on bad args.
 * Does not free heap storage (caller owns slots). No parent wires.
 */
int
gj_dlist_u32_remove_u(uint32_t *pVal, size_t *pNext, size_t *pPrev,
                      size_t *pHead, size_t *pFreeHead, size_t iIdx,
                      uint32_t *pOut)
{
	(void)NULL;

	if (pNext == NULL || pPrev == NULL || pHead == NULL ||
	    pFreeHead == NULL || iIdx == B3938_NIL) {
		return -1;
	}
	if (pOut != NULL && pVal == NULL) {
		return -1;
	}

	if (pOut != NULL) {
		*pOut = pVal[iIdx];
	}

	b3938_unlink(pNext, pPrev, pHead, iIdx);
	b3938_free(pNext, pFreeHead, iIdx);
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_dlist_u32_remove_u(uint32_t *pVal, size_t *pNext, size_t *pPrev,
                            size_t *pHead, size_t *pFreeHead, size_t iIdx,
                            uint32_t *pOut)
    __attribute__((alias("gj_dlist_u32_remove_u")));
