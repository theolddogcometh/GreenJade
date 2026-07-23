/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch438: freestanding uint32 deque push helpers
 * on caller storage (ring layout; head/tail/count owned by caller).
 *
 * Distinct from batch437 FIFO queue: this surface only adds values at
 * either end (front or back). Single-threaded / cooperative use.
 *
 * Surface (unique symbols):
 *   int gj_deque_u32_push_front(uint32_t *buf, size_t cap, size_t *head,
 *                               size_t *tail, size_t *count, uint32_t v);
 *     — Insert v before the current front (tail). Returns 0 on success,
 *       -1 if full or bad args.
 *   int gj_deque_u32_push_back(uint32_t *buf, size_t cap, size_t *head,
 *                              size_t *tail, size_t *count, uint32_t v);
 *     — Insert v after the current back (head). Returns 0 on success,
 *       -1 if full or bad args.
 *   __gj_deque_u32_push_front / __gj_deque_u32_push_back  (aliases)
 *   __libcgj_batch438_marker = "libcgj-batch438"
 *
 * Semantics (same ring layout as batch437 queue):
 *   head  — next back-write index (mod cap)
 *   tail  — front / next-read index (mod cap)
 *   count — elements currently stored (0..cap)
 *   Caller may zero head/tail/count before first use.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch438_marker[] = "libcgj-batch438";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deque_u32_push_front — insert one uint32_t at the front.
 *
 * Retreats tail modulo cap, then stores v at the new front slot.
 * Returns 0 on success; -1 if full, zero capacity, or NULL required args.
 */
int
gj_deque_u32_push_front(uint32_t *pBuf, size_t cCap, size_t *pHead,
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

/*
 * gj_deque_u32_push_back — insert one uint32_t at the back.
 *
 * Stores v at head, advances head modulo cap, increments count.
 * Returns 0 on success; -1 if full, zero capacity, or NULL required args.
 * (tail accepted for API symmetry with push_front; unused here.)
 */
int
gj_deque_u32_push_back(uint32_t *pBuf, size_t cCap, size_t *pHead,
                       size_t *pTail, size_t *pCount, uint32_t u32V)
{
	size_t iH;

	(void)pTail;

	if (pBuf == NULL || pHead == NULL || pCount == NULL || cCap == 0u) {
		return -1;
	}
	if (*pCount >= cCap) {
		return -1;
	}

	iH = *pHead;
	if (iH >= cCap) {
		/* Corrupt cursor — refuse rather than write out of bounds. */
		return -1;
	}
	pBuf[iH] = u32V;
	iH++;
	if (iH >= cCap) {
		iH = 0u;
	}
	*pHead = iH;
	(*pCount)++;
	return 0;
}

/* ---- underscored aliases ----------------------------------------------- */

int __gj_deque_u32_push_front(uint32_t *pBuf, size_t cCap, size_t *pHead,
                              size_t *pTail, size_t *pCount, uint32_t u32V)
    __attribute__((alias("gj_deque_u32_push_front")));
int __gj_deque_u32_push_back(uint32_t *pBuf, size_t cCap, size_t *pHead,
                             size_t *pTail, size_t *pCount, uint32_t u32V)
    __attribute__((alias("gj_deque_u32_push_back")));
