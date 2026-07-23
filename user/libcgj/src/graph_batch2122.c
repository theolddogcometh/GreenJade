/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2122: freestanding uint32 FIFO single pop.
 *
 * Surface (unique symbols):
 *   int gj_fifo_u32_pop(uint32_t *buf, size_t cap, size_t *head,
 *                       size_t *tail, size_t *count, uint32_t *out);
 *     - Dequeue one uint32_t from tail into *out. Returns 0 on success,
 *       -1 if empty or bad args.
 *   int __gj_fifo_u32_pop  (alias)
 *   __libcgj_batch2122_marker = "libcgj-batch2122"
 *
 * FIFO design (count-based head/tail, same model as batch437 queue_u32
 * and batch1802 ring8):
 *   head  - next write index (accepted for API symmetry; unused)
 *   tail  - next read index (mod cap)
 *   count - elements currently stored (0..cap)
 *   empty when count == 0
 *
 * Post-2100 fifo_u32 exclusive wave (2121-2130). Distinct from
 * gj_queue_u32_pop / gj_ring_u32_pop / gj_ring8_pop - unique surface;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2122_marker[] = "libcgj-batch2122";

/* ---- freestanding helpers ---------------------------------------------- */

/* Advance index by one with modular wrap (any cap > 0). */
static size_t
b2122_next(size_t iIdx, size_t cCap)
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
 * gj_fifo_u32_pop - dequeue one uint32_t into *out.
 *
 * Advances tail modulo cap and decrements count. Does not touch head.
 * Returns 0 on success; -1 if empty, zero capacity, or NULL required args.
 */
int
gj_fifo_u32_pop(uint32_t *pBuf, size_t cCap, size_t *pHead, size_t *pTail,
                size_t *pCount, uint32_t *pOut)
{
	size_t iT;

	(void)NULL;
	(void)pHead;

	if (pBuf == NULL || pTail == NULL || pCount == NULL || pOut == NULL ||
	    cCap == 0u) {
		return -1;
	}
	if (*pCount == 0u) {
		return -1;
	}

	iT = *pTail;
	if (iT >= cCap) {
		/* Corrupt cursor - refuse rather than read out of bounds. */
		return -1;
	}
	*pOut = pBuf[iT];
	*pTail = b2122_next(iT, cCap);
	(*pCount)--;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_fifo_u32_pop(uint32_t *pBuf, size_t cCap, size_t *pHead,
                      size_t *pTail, size_t *pCount, uint32_t *pOut)
    __attribute__((alias("gj_fifo_u32_pop")));
