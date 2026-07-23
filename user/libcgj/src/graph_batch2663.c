/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2663: freestanding uint32 queue pop on caller
 * storage (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_queue_u32_pop_u(uint32_t *buf, size_t cap, size_t *h, size_t *t,
 *                          size_t *n, uint32_t *out);
 *     - Dequeue one uint32_t from tail into *out. Returns 0 on success,
 *       -1 if empty or bad args.
 *   int __gj_queue_u32_pop_u  (alias)
 *   __libcgj_batch2663_marker = "libcgj-batch2663"
 *
 * Queue design (count-based head/tail):
 *   h / *h — next write index (accepted for API symmetry; unused)
 *   t / *t — next read index (mod cap)
 *   n / *n — elements currently stored (0..cap)
 *   empty when *n == 0
 *
 * Distinct from gj_queue_u32_pop (batch437) / gj_fifo_u32_pop
 * (batch2122) by public _u name — unique surface; no multi-def.
 * Queue/stack u32 exclusive wave (2661-2670). Caller buffers only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2663_marker[] = "libcgj-batch2663";

/* ---- freestanding helpers ---------------------------------------------- */

/* Advance index by one with modular wrap (any cap > 0). */
static size_t
b2663_next(size_t iIdx, size_t cCap)
{
	size_t iNext;

	iNext = iIdx + 1u;
	if (iNext >= cCap) {
		iNext = 0u;
	}
	return iNext;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_queue_u32_pop_u - dequeue one uint32_t into *out.
 *
 * Advances tail modulo cap and decrements count. Does not touch head.
 * Returns 0 on success; -1 if empty, zero capacity, or NULL required args.
 * (h is accepted for API symmetry with push; unused here.)
 * *out and cursors are left untouched on failure.
 */
int
gj_queue_u32_pop_u(uint32_t *pBuf, size_t cCap, size_t *pH, size_t *pT,
                   size_t *pN, uint32_t *pOut)
{
	size_t iT;

	(void)NULL;
	(void)pH;

	if (pBuf == NULL || pT == NULL || pN == NULL || pOut == NULL ||
	    cCap == 0u) {
		return -1;
	}
	if (*pN == 0u) {
		return -1;
	}

	iT = *pT;
	if (iT >= cCap) {
		/* Corrupt cursor — refuse rather than read out of bounds. */
		return -1;
	}
	*pOut = pBuf[iT];
	*pT = b2663_next(iT, cCap);
	(*pN)--;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_queue_u32_pop_u(uint32_t *pBuf, size_t cCap, size_t *pH, size_t *pT,
                         size_t *pN, uint32_t *pOut)
    __attribute__((alias("gj_queue_u32_pop_u")));
