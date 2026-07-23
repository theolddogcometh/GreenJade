/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2662: freestanding uint32 queue push on caller
 * storage (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_queue_u32_push_u(uint32_t *buf, size_t cap, size_t *h, size_t *t,
 *                           size_t *n, uint32_t v);
 *     - Enqueue one uint32_t at head. Returns 0 on success, -1 if full
 *       or bad args.
 *   int __gj_queue_u32_push_u  (alias)
 *   __libcgj_batch2662_marker = "libcgj-batch2662"
 *
 * Queue design (count-based head/tail):
 *   h / *h — next write index (mod cap)
 *   t / *t — next read index (accepted for API symmetry; unused)
 *   n / *n — elements currently stored (0..cap)
 *   full when *n == cap
 *
 * Distinct from gj_queue_u32_push (batch437) / gj_fifo_u32_push
 * (batch2121) by public _u name — unique surface; no multi-def.
 * Queue/stack u32 exclusive wave (2661-2670). Caller buffers only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2662_marker[] = "libcgj-batch2662";

/* ---- freestanding helpers ---------------------------------------------- */

/* Advance index by one with modular wrap (any cap > 0). */
static size_t
b2662_next(size_t iIdx, size_t cCap)
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
 * gj_queue_u32_push_u - enqueue one uint32_t into a count-based queue.
 *
 * Advances head modulo cap and increments count. Does not touch tail.
 * Returns 0 on success; -1 if full, zero capacity, or NULL required args.
 * (t is accepted for API symmetry with pop; unused here.)
 */
int
gj_queue_u32_push_u(uint32_t *pBuf, size_t cCap, size_t *pH, size_t *pT,
                    size_t *pN, uint32_t u32V)
{
	size_t iH;

	(void)NULL;
	(void)pT;

	if (pBuf == NULL || pH == NULL || pN == NULL || cCap == 0u) {
		return -1;
	}
	if (*pN >= cCap) {
		return -1;
	}

	iH = *pH;
	if (iH >= cCap) {
		/* Corrupt cursor — refuse rather than write out of bounds. */
		return -1;
	}
	pBuf[iH] = u32V;
	*pH = b2662_next(iH, cCap);
	(*pN)++;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_queue_u32_push_u(uint32_t *pBuf, size_t cCap, size_t *pH, size_t *pT,
                          size_t *pN, uint32_t u32V)
    __attribute__((alias("gj_queue_u32_push_u")));
