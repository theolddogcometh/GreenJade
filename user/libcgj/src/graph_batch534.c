/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch534: classic uint32_t ring pop on caller storage.
 *
 * Pairs with concurrent batch533 (push side). Same ring convention as
 * graph_batch533.c (classic head/tail, one empty slot, no count field):
 *   *head — next read index  (0 .. cap-1); advanced by this pop
 *   *tail — next write index (0 .. cap-1); advanced by batch533 push
 *   empty when *head == *tail
 *   full  when (*tail + 1) mod cap == *head
 *   usable capacity is (cap - 1)
 *
 * Surface (unique; pop half only — does NOT redefine batch533 push):
 *   int gj_ring_u32_pop(uint32_t *buf, size_t cap, size_t *head,
 *                       size_t *tail, uint32_t *out);
 *     — Dequeue one uint32_t from *head into *out. Advances *head mod cap.
 *       Returns 0 on success; -1 if empty, bad args, or corrupt cursors.
 *   int __gj_ring_u32_pop  (alias)
 *   __libcgj_batch534_marker = "libcgj-batch534"
 *
 * Indices use modular wrap (next = i+1; if next >= cap then 0), matching
 * batch533. Single-threaded / cooperative use (no atomics).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch534_marker[] = "libcgj-batch534";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_u32_pop — dequeue one uint32_t from a classic head/tail ring.
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
gj_ring_u32_pop(uint32_t *buf, size_t cap, size_t *head, size_t *tail,
                uint32_t *out)
{
	size_t h;
	size_t t;
	size_t next;

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

	/* next read index modulo cap (wrap without requiring power-of-two). */
	next = h + 1u;
	if (next >= cap) {
		next = 0u;
	}
	*head = next;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ring_u32_pop(uint32_t *buf, size_t cap, size_t *head, size_t *tail,
                      uint32_t *out)
    __attribute__((alias("gj_ring_u32_pop")));
