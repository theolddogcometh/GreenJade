/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2664: freestanding uint32 queue front peek
 * (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_queue_u32_peek_u(const uint32_t *buf, size_t cap, size_t t,
 *                           size_t n, uint32_t *out);
 *     - Read the oldest element (at tail) into *out without dequeue.
 *       Returns 0 on success; -1 if empty or bad args. Does not mutate
 *       cursors or buffer.
 *   int __gj_queue_u32_peek_u  (alias)
 *   __libcgj_batch2664_marker = "libcgj-batch2664"
 *
 * Queue design (count-based head/tail):
 *   t / tail — next read index (mod cap); passed by value
 *   n / count — elements currently stored; empty when n == 0
 *
 * Distinct from gj_fifo_u32_peek (batch2123) / gj_ring_u32_peek_u
 * (batch2434) by public queue_u32_peek_u name — unique surface;
 * no multi-def. Queue/stack u32 exclusive wave (2661-2670).
 * Caller buffers only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2664_marker[] = "libcgj-batch2664";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if cursor and occupancy look valid for a non-empty peek. */
static int
b2664_ok(size_t cCap, size_t iTail, size_t cN)
{
	if (cCap == 0u) {
		return 0;
	}
	if (cN == 0u || cN > cCap) {
		return 0;
	}
	if (iTail >= cCap) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_queue_u32_peek_u - non-destructive read of the oldest queue element.
 *
 * buf/cap: caller-owned element storage
 * t:       next-read index (by value; not modified)
 * n:       live occupancy
 * out:     destination for the peeked value (must be non-NULL)
 *
 * Returns 0 on success with *out written. Returns -1 if empty, NULL
 * required args, cap == 0, or corrupt tail. *out is left untouched on
 * failure.
 */
int
gj_queue_u32_peek_u(const uint32_t *pBuf, size_t cCap, size_t iT, size_t cN,
                    uint32_t *pOut)
{
	(void)NULL;

	if (pBuf == NULL || pOut == NULL) {
		return -1;
	}
	if (b2664_ok(cCap, iT, cN) == 0) {
		return -1;
	}

	*pOut = pBuf[iT];
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_queue_u32_peek_u(const uint32_t *pBuf, size_t cCap, size_t iT,
                          size_t cN, uint32_t *pOut)
    __attribute__((alias("gj_queue_u32_peek_u")));
