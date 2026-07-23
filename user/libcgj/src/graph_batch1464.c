/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1464: classic uint64_t ring pop on caller storage.
 *
 * Pairs with batch1463 (push side). Same ring convention as batch1463
 * (classic head/tail, one empty slot, no count field):
 *   *head — next read index  (0 .. cap-1); advanced by this pop
 *   *tail — next write index (0 .. cap-1); advanced by batch1463 push
 *   empty when *head == *tail
 *   full  when (*tail + 1) mod cap == *head
 *   usable capacity is (cap - 1)
 *
 * Surface (unique; pop half only — does NOT redefine batch1463 push,
 * batch185 gj_u64ring_pop, or batch884 gj_spsc_try_read_u64):
 *   int gj_ring_u64_pop(uint64_t *buf, size_t cap, size_t *head,
 *                       size_t *tail, uint64_t *out);
 *     — Dequeue one uint64_t from *head into *out. Advances *head mod cap.
 *       Returns 0 on success; -1 if empty, bad args, or corrupt cursors.
 *   int __gj_ring_u64_pop  (alias)
 *   __libcgj_batch1464_marker = "libcgj-batch1464"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1464_marker[] = "libcgj-batch1464";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1464_next(size_t i, size_t cap)
{
	size_t n;

	n = i + 1u;
	if (n >= cap) {
		n = 0u;
	}
	return n;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_u64_pop — dequeue one uint64_t from a classic head/tail ring.
 *
 * buf:  caller-owned element storage of length cap
 * cap:  number of slots in buf (prefer power-of-two; must be > 1 to hold data)
 * head: next-read index (advanced on success)
 * tail: next-write index (not modified by pop; used for empty detection)
 * out:  destination for the dequeued value
 *
 * Returns 0 on success with *out written and *head advanced.
 * Returns -1 if empty, any required pointer is NULL, cap < 2, or a cursor
 * is out of range (corrupt). *out and *head are left untouched on failure.
 * Does not touch *tail.
 */
int
gj_ring_u64_pop(uint64_t *buf, size_t cap, size_t *head, size_t *tail,
                uint64_t *out)
{
	size_t h;
	size_t t;

	if (buf == NULL || head == NULL || tail == NULL || out == NULL ||
	    cap < 2u) {
		return -1;
	}

	h = *head;
	t = *tail;
	if (h >= cap || t >= cap) {
		/* Corrupt cursor — refuse rather than read out of bounds. */
		return -1;
	}

	/* Empty: head (read) has caught up with tail (write). */
	if (h == t) {
		return -1;
	}

	*out = buf[h];
	*head = b1464_next(h, cap);
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ring_u64_pop(uint64_t *buf, size_t cap, size_t *head, size_t *tail,
                      uint64_t *out)
    __attribute__((alias("gj_ring_u64_pop")));
