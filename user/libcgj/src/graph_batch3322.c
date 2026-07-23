/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3322: freestanding uint32_t deque push-front (_u).
 *
 * Surface (unique symbols):
 *   int gj_deque_u32_push_front_u(uint32_t *buf, size_t cap, size_t *head,
 *                                 size_t *tail, size_t *count, uint32_t v);
 *     - Insert v before the current front (tail). Returns 0 on success,
 *       -1 if full or bad args.
 *   int __gj_deque_u32_push_front_u  (alias)
 *   __libcgj_batch3322_marker = "libcgj-batch3322"
 *
 * Exclusive continuum CREATE-ONLY (3321-3330). Distinct from
 * gj_deque_u32_push_front (batch438) and gj_u32_deque_push_front
 * (batch703) — unique _u surface only; no multi-def. No parent wires.
 *
 * Ring layout (count-based, same as batch438 / batch3321 init):
 *   head  — next back-write index (mod cap)
 *   tail  — front / next-read index (mod cap)
 *   count — elements currently stored (0..cap)
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3322_marker[] = "libcgj-batch3322";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deque_u32_push_front_u - insert one uint32_t at the front.
 *
 * Retreats tail modulo cap, then stores v at the new front slot.
 * Returns 0 on success; -1 if full, zero capacity, or NULL required args.
 * (head accepted for API symmetry with push_back; unused here.)
 */
int
gj_deque_u32_push_front_u(uint32_t *pBuf, size_t cCap, size_t *pHead,
                          size_t *pTail, size_t *pCount, uint32_t u32V)
{
	size_t iT;

	(void)pHead;

	if (pBuf == NULL || pTail == NULL || pCount == NULL || cCap == 0u) {
		return -1;
	}
	if (*pCount >= cCap) {
		return -1;
	}

	iT = *pTail;
	if (iT >= cCap) {
		/* Corrupt cursor — refuse rather than write out of bounds. */
		return -1;
	}

	/* Retreat tail: front slot is one before the previous front. */
	if (iT == 0u) {
		iT = cCap - 1u;
	} else {
		iT--;
	}
	pBuf[iT] = u32V;
	*pTail = iT;
	(*pCount)++;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_deque_u32_push_front_u(uint32_t *pBuf, size_t cCap, size_t *pHead,
                                size_t *pTail, size_t *pCount, uint32_t u32V)
    __attribute__((alias("gj_deque_u32_push_front_u")));
