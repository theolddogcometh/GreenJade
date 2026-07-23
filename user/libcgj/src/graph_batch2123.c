/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2123: freestanding uint32 FIFO front peek.
 *
 * Surface (unique symbols):
 *   int gj_fifo_u32_peek(const uint32_t *buf, size_t cap, size_t tail,
 *                        size_t count, uint32_t *out);
 *     - Read the oldest element (at tail) into *out without dequeue.
 *       Returns 0 on success; -1 if empty or bad args. Does not mutate
 *       cursors or buffer.
 *   int __gj_fifo_u32_peek  (alias)
 *   __libcgj_batch2123_marker = "libcgj-batch2123"
 *
 * FIFO design (count-based head/tail):
 *   tail  - next read index (mod cap); passed by value
 *   count - elements currently stored; empty when count == 0
 *
 * Post-2100 fifo_u32 exclusive wave (2121-2130). Distinct from
 * gj_ring_u8_peek / gj_ring_u32_peek_nth - unique surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2123_marker[] = "libcgj-batch2123";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if cursor and occupancy look valid for a non-empty peek. */
static int
b2123_ok(size_t cCap, size_t iTail, size_t cCount)
{
	if (cCap == 0u) {
		return 0;
	}
	if (cCount == 0u || cCount > cCap) {
		return 0;
	}
	if (iTail >= cCap) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fifo_u32_peek - non-destructive read of the oldest FIFO element.
 *
 * buf/cap: caller-owned element storage
 * tail:    next-read index (by value; not modified)
 * count:   live occupancy
 * out:     destination for the peeked value (must be non-NULL)
 *
 * Returns 0 on success with *out written. Returns -1 if empty, NULL
 * required args, cap == 0, or corrupt tail. *out is left untouched on
 * failure.
 */
int
gj_fifo_u32_peek(const uint32_t *pBuf, size_t cCap, size_t iTail,
                 size_t cCount, uint32_t *pOut)
{
	(void)NULL;

	if (pBuf == NULL || pOut == NULL) {
		return -1;
	}
	if (b2123_ok(cCap, iTail, cCount) == 0) {
		return -1;
	}

	*pOut = pBuf[iTail];
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_fifo_u32_peek(const uint32_t *pBuf, size_t cCap, size_t iTail,
                       size_t cCount, uint32_t *pOut)
    __attribute__((alias("gj_fifo_u32_peek")));
