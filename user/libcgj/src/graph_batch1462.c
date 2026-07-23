/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1462: uint32_t ring peek at nth oldest element.
 *
 * Head/tail live in caller-owned size_t slots (no struct, no heap, no count).
 * Buffer is also caller-owned. Single-threaded / cooperative use (no atomics).
 * Does not mutate cursors or buffer.
 *
 * Surface (unique; pairs with batch533/534/1461 classic head/tail ring):
 *   int gj_ring_u32_peek_nth(const uint32_t *buf, size_t cap, size_t head,
 *                            size_t tail, size_t n, uint32_t *out);
 *     — Read the n-th enqueued element (0 = oldest at head) into *out
 *       without dequeue. Returns 0 on success; -1 if out of range, empty,
 *       bad args, or corrupt cursors.
 *   int __gj_ring_u32_peek_nth  (alias)
 *   __libcgj_batch1462_marker = "libcgj-batch1462"
 *
 * Ring design (classic head/tail, one empty slot) — same as batch533/534:
 *   head — next read index  (0 .. cap-1)
 *   tail — next write index (0 .. cap-1)
 *   empty when head == tail
 *   used  = (tail - head) mod cap   (0 .. cap-1)
 *   slot n is at (head + n) mod cap when n < used
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1462_marker[] = "libcgj-batch1462";

/* ---- freestanding helpers ---------------------------------------------- */

/* Occupancy of classic one-empty-slot ring: (tail - head) mod cap. */
static size_t
b1462_used(size_t head, size_t tail, size_t cap)
{
	if (tail >= head) {
		return tail - head;
	}
	return (cap - head) + tail;
}

/* Index of n-th element from head with modular wrap. */
static size_t
b1462_index(size_t head, size_t n, size_t cap)
{
	size_t i;

	i = head + n;
	if (i >= cap) {
		i -= cap;
		/* n < used <= cap-1, so at most one wrap. */
		if (i >= cap) {
			i %= cap;
		}
	}
	return i;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_u32_peek_nth — non-destructive read of the n-th ring element.
 *
 * buf:  caller-owned element storage of length cap
 * cap:  number of slots in buf (must be > 1)
 * head: next-read index (passed by value; not modified)
 * tail: next-write index (passed by value; not modified)
 * n:    zero-based offset from oldest (head); 0 peeks front
 * out:  destination for the peeked value (must be non-NULL)
 *
 * Returns 0 on success with *out written. Returns -1 if empty, n past
 * occupancy, NULL required args, cap < 2, or corrupt cursors. *out is
 * left untouched on failure.
 */
int
gj_ring_u32_peek_nth(const uint32_t *buf, size_t cap, size_t head, size_t tail,
                     size_t n, uint32_t *out)
{
	size_t cUsed;
	size_t iSlot;

	if (buf == NULL || out == NULL || cap < 2u) {
		return -1;
	}
	if (head >= cap || tail >= cap) {
		return -1;
	}

	cUsed = b1462_used(head, tail, cap);
	if (n >= cUsed) {
		return -1;
	}

	iSlot = b1462_index(head, n, cap);
	*out = buf[iSlot];
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ring_u32_peek_nth(const uint32_t *buf, size_t cap, size_t head,
                           size_t tail, size_t n, uint32_t *out)
    __attribute__((alias("gj_ring_u32_peek_nth")));
