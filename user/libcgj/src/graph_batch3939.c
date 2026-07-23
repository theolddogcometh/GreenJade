/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3939: freestanding index-based doubly-linked
 * uint32_t list length (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   size_t gj_dlist_u32_len_u(const size_t *next, size_t head, size_t cap);
 *     - Count live nodes by walking next[] from head until B3939_NIL.
 *       Returns 0 when next is NULL, cap is 0, or head is NIL. Bounds
 *       each index against cap and stops on corrupt chains (index >= cap).
 *       Visits at most cap links (cycle-safe).
 *   size_t __gj_dlist_u32_len_u  (alias)
 *   __libcgj_batch3939_marker = "libcgj-batch3939"
 *
 * Exclusive continuum CREATE-ONLY (3931-3940). Distinct from
 * gj_list_len_n (batch2337), gj_list_u32_len_u (batch3338), and
 * gj_deque_u64_len_u (batch3933) — unique dlist_u32_len_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3939_marker[] = "libcgj-batch3939";

/* Empty / terminal sentinel. */
#define B3939_NIL (~(size_t)0)

/* ---- freestanding helpers ---------------------------------------------- */

/* Count nodes along next[] from iHead, capped at cCap steps. */
static size_t
b3939_len(const size_t *pNext, size_t iHead, size_t cCap)
{
	size_t iCur;
	size_t cNodes;

	if (pNext == NULL || cCap == 0u || iHead == B3939_NIL) {
		return 0u;
	}

	iCur = iHead;
	cNodes = 0u;
	while (iCur != B3939_NIL && cNodes < cCap) {
		if (iCur >= cCap) {
			/* Corrupt cursor — report nodes counted so far. */
			break;
		}
		cNodes++;
		iCur = pNext[iCur];
	}
	return cNodes;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dlist_u32_len_u - live node count of an index-based dlist.
 *
 * next: link array (NULL -> 0)
 * head: live list head (B3939_NIL -> 0)
 * cap:  slot table capacity (0 -> 0); also the visit budget
 *
 * O(n) walk via next only (prev unused). Does not call libc.
 * No parent wires.
 */
size_t
gj_dlist_u32_len_u(const size_t *pNext, size_t iHead, size_t cCap)
{
	(void)NULL;
	return b3939_len(pNext, iHead, cCap);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_dlist_u32_len_u(const size_t *pNext, size_t iHead, size_t cCap)
    __attribute__((alias("gj_dlist_u32_len_u")));
