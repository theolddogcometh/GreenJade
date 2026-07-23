/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch281: byte ring buffer helpers on caller storage.
 *
 * Head/tail/count live in caller-owned size_t slots (no struct, no heap).
 * Buffer is also caller-owned. Single-threaded / cooperative use (no atomics).
 *
 * Surface (unique for this design; distinct from batch117 SPSC struct ring):
 *   void gj_ring_u8_init(size_t *head, size_t *tail, size_t *count);
 *     — Zero head, tail, and count. NULL any pointer → no-op.
 *   int  gj_ring_u8_push(uint8_t *buf, size_t cap, size_t *head, size_t *tail,
 *                     size_t *count, uint8_t b);
 *     — Enqueue one byte at head. Returns 0 on success, -1 if full or bad args.
 *   int  gj_ring_u8_pop(uint8_t *buf, size_t cap, size_t *head, size_t *tail,
 *                    size_t *count, uint8_t *out);
 *     — Dequeue one byte from tail into *out. Returns 0 on success, -1 if
 *       empty or bad args.
 *   __gj_ring_u8_init / __gj_ring_u8_push / __gj_ring_u8_pop  (aliases)
 *   __libcgj_batch281_marker = "libcgj-batch281"
 *
 * Semantics:
 *   head  — next write index (mod cap)
 *   tail  — next read index (mod cap)
 *   count — bytes currently stored (0..cap)
 *   full when count == cap; empty when count == 0.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch281_marker[] = "libcgj-batch281";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_u8_init — reset ring cursors on caller storage.
 * Leaves buf contents untouched (caller may reuse or zero separately).
 */
void
gj_ring_u8_init(size_t *head, size_t *tail, size_t *count)
{
	if (head == NULL || tail == NULL || count == NULL) {
		return;
	}
	*head = 0u;
	*tail = 0u;
	*count = 0u;
}

/*
 * gj_ring_u8_push — enqueue one byte.
 *
 * Advances head modulo cap and increments count. Does not touch tail.
 * Returns 0 on success; -1 if full, zero capacity, or NULL required args.
 * (tail is accepted for API symmetry with pop; unused here.)
 */
int
gj_ring_u8_push(uint8_t *buf, size_t cap, size_t *head, size_t *tail,
             size_t *count, uint8_t b)
{
	size_t h;

	(void)tail;

	if (buf == NULL || head == NULL || count == NULL || cap == 0u) {
		return -1;
	}
	if (*count >= cap) {
		return -1;
	}

	h = *head;
	if (h >= cap) {
		/* Corrupt cursor — refuse rather than write out of bounds. */
		return -1;
	}
	buf[h] = b;
	h++;
	if (h >= cap) {
		h = 0u;
	}
	*head = h;
	(*count)++;
	return 0;
}

/*
 * gj_ring_u8_pop — dequeue one byte into *out.
 *
 * Advances tail modulo cap and decrements count. Does not touch head.
 * Returns 0 on success; -1 if empty, zero capacity, or NULL required args.
 * (head is accepted for API symmetry with push; unused here.)
 */
int
gj_ring_u8_pop(uint8_t *buf, size_t cap, size_t *head, size_t *tail,
            size_t *count, uint8_t *out)
{
	size_t t;

	(void)head;

	if (buf == NULL || tail == NULL || count == NULL || out == NULL ||
	    cap == 0u) {
		return -1;
	}
	if (*count == 0u) {
		return -1;
	}

	t = *tail;
	if (t >= cap) {
		/* Corrupt cursor — refuse rather than read out of bounds. */
		return -1;
	}
	*out = buf[t];
	t++;
	if (t >= cap) {
		t = 0u;
	}
	*tail = t;
	(*count)--;
	return 0;
}

void __gj_ring_u8_init(size_t *head, size_t *tail, size_t *count)
    __attribute__((alias("gj_ring_u8_init")));
int __gj_ring_u8_push(uint8_t *buf, size_t cap, size_t *head, size_t *tail,
                   size_t *count, uint8_t b)
    __attribute__((alias("gj_ring_u8_push")));
int __gj_ring_u8_pop(uint8_t *buf, size_t cap, size_t *head, size_t *tail,
                  size_t *count, uint8_t *out)
    __attribute__((alias("gj_ring_u8_pop")));
