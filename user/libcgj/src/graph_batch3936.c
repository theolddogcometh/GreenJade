/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3936: freestanding index-based singly-linked
 * uint32_t list find (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   size_t gj_slist_u32_find_u(const uint32_t *val, const size_t *next,
 *                              size_t head, size_t cap, uint32_t v);
 *     - Walk the live chain from head; return the first slot index with
 *       val[i] == v. Returns B3936_NIL (~(size_t)0) if not found, head
 *       is NIL, or args are bad. Bounds-checks each index against cap
 *       and refuses corrupt chains (index >= cap).
 *   size_t __gj_slist_u32_find_u  (alias)
 *   __libcgj_batch3936_marker = "libcgj-batch3936"
 *
 * Exclusive continuum CREATE-ONLY (3931-3940). Distinct from
 * gj_list_u32_find_u (batch3337), gj_arr_u32_find_u (batch2869), and
 * gj_u32_find (batch611) — unique slist_u32_find_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3936_marker[] = "libcgj-batch3936";

/* Not-found / empty / corrupt-chain sentinel. */
#define B3936_NIL (~(size_t)0)

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Walk from iHead along next[] looking for u32V. Cap bounds every index.
 * Returns matching slot or B3936_NIL. Does not detect cycles beyond cap
 * steps (visits at most cCap links).
 */
static size_t
b3936_find(const uint32_t *pVal, const size_t *pNext, size_t iHead,
           size_t cCap, uint32_t u32V)
{
	size_t iCur;
	size_t cSteps;

	if (pVal == NULL || pNext == NULL || cCap == 0u) {
		return B3936_NIL;
	}

	iCur = iHead;
	cSteps = 0u;
	while (iCur != B3936_NIL && cSteps < cCap) {
		if (iCur >= cCap) {
			return B3936_NIL;
		}
		if (pVal[iCur] == u32V) {
			return iCur;
		}
		iCur = pNext[iCur];
		cSteps++;
	}
	return B3936_NIL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_slist_u32_find_u - first live slot holding v, or B3936_NIL if absent.
 *
 * val / next: parallel slot arrays
 * head:       live list head (B3936_NIL -> not found)
 * cap:        slot table capacity (0 -> not found)
 * v:          value to locate
 *
 * Multiple matches -> earliest in list order (from head). O(n) walk.
 * No parent wires. Does not call libc.
 */
size_t
gj_slist_u32_find_u(const uint32_t *pVal, const size_t *pNext, size_t iHead,
                    size_t cCap, uint32_t u32V)
{
	(void)NULL;
	return b3936_find(pVal, pNext, iHead, cCap, u32V);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_slist_u32_find_u(const uint32_t *pVal, const size_t *pNext,
                             size_t iHead, size_t cCap, uint32_t u32V)
    __attribute__((alias("gj_slist_u32_find_u")));
