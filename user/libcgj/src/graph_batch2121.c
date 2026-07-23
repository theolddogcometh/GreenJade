/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2121: freestanding uint32 FIFO single push.
 *
 * Surface (unique symbols):
 *   int gj_fifo_u32_push(uint32_t *buf, size_t cap, size_t *head,
 *                        size_t *tail, size_t *count, uint32_t v);
 *     - Enqueue one uint32_t at head. Returns 0 on success, -1 if full
 *       or bad args.
 *   int __gj_fifo_u32_push  (alias)
 *   __libcgj_batch2121_marker = "libcgj-batch2121"
 *
 * FIFO design (count-based head/tail, same model as batch437 queue_u32
 * and batch1801 ring8):
 *   head  - next write index (mod cap)
 *   tail  - next read index (accepted for API symmetry; unused)
 *   count - elements currently stored (0..cap)
 *   full when count == cap
 *
 * Post-2100 fifo_u32 exclusive wave (2121-2130). Distinct from
 * gj_queue_u32_push / gj_ring_u32_push / gj_ring8_push - unique surface;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2121_marker[] = "libcgj-batch2121";

/* ---- freestanding helpers ---------------------------------------------- */

/* Advance index by one with modular wrap (any cap > 0). */
static size_t
b2121_next(size_t iIdx, size_t cCap)
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
 * gj_fifo_u32_push - enqueue one uint32_t into a count-based FIFO.
 *
 * Advances head modulo cap and increments count. Does not touch tail.
 * Returns 0 on success; -1 if full, zero capacity, or NULL required args.
 */
int
gj_fifo_u32_push(uint32_t *pBuf, size_t cCap, size_t *pHead, size_t *pTail,
                 size_t *pCount, uint32_t u32V)
{
	size_t iH;

	(void)NULL;
	(void)pTail;

	if (pBuf == NULL || pHead == NULL || pCount == NULL || cCap == 0u) {
		return -1;
	}
	if (*pCount >= cCap) {
		return -1;
	}

	iH = *pHead;
	if (iH >= cCap) {
		/* Corrupt cursor - refuse rather than write out of bounds. */
		return -1;
	}
	pBuf[iH] = u32V;
	*pHead = b2121_next(iH, cCap);
	(*pCount)++;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_fifo_u32_push(uint32_t *pBuf, size_t cCap, size_t *pHead,
                       size_t *pTail, size_t *pCount, uint32_t u32V)
    __attribute__((alias("gj_fifo_u32_push")));
