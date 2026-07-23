/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch705: uint32_t classic ring deque pop-front.
 *
 * Head/tail live in caller-owned size_t slots (no struct, no heap, no count).
 * Buffer is also caller-owned. Single-threaded / cooperative use (no atomics).
 *
 * Surface (unique; does NOT redefine batch678 gj_u32_queue_pop,
 * batch533/534 gj_ring_u32_*, batch437 gj_queue_u32_pop,
 * batch438 gj_deque_u32_push_*, batch281 gj_ring_u8_*, batch185
 * gj_u64ring_* — avoid multi-def):
 *   int gj_u32_deque_pop_front(uint32_t *buf, size_t cap, size_t *head,
 *                              size_t *tail, uint32_t *out);
 *     — Pop one uint32_t from the front (*head) into *out. Returns 0 (OK)
 *       on success, -1 (FAIL) if empty or NULL args / zero-or-unit capacity /
 *       corrupt cursors.
 *   int __gj_u32_deque_pop_front  (alias)
 *   __libcgj_batch705_marker = "libcgj-batch705"
 *
 * Ring design (classic head/tail, one empty slot):
 *   *head — next front/read index  (0 .. cap-1); advanced by this pop
 *   *tail — next back/write index  (0 .. cap-1); advanced by push-back
 *   empty when *head == *tail
 *   full  when (*tail + 1) mod cap == *head
 *   used  = (*tail - *head) mod cap   (0 .. cap-1)
 *   usable capacity is (cap - 1); one slot is reserved so full and empty
 *   remain distinguishable without a separate count field.
 *
 * Indices use modular wrap (next = i+1; if next >= cap then 0). A power-of-two
 * cap is preferred (cheap mask equivalent, natural alignment) but any cap > 1
 * is accepted. cap == 0 or cap == 1 cannot hold an element under this design.
 *
 * Init (caller): *head = *tail = 0; buf[0..cap-1] is caller-owned storage.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch705_marker[] = "libcgj-batch705";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_deque_pop_front — remove one uint32_t from the front of a classic
 * head/tail ring deque.
 *
 * buf:  caller-owned element storage of length cap
 * cap:  number of slots in buf (prefer power-of-two; must be > 1 to pop)
 * head: next-front/read index (advanced on success)
 * tail: next-back/write index (not modified by pop; used for empty detection)
 * out:  destination for the popped value (must be non-NULL)
 *
 * Returns 0 (OK) on success; -1 (FAIL) if empty, NULL required args, cap < 2,
 * or a cursor is out of range (corrupt). *head and *out are left untouched on
 * failure. Does not touch *tail.
 */
int
gj_u32_deque_pop_front(uint32_t *buf, size_t cap, size_t *head, size_t *tail,
                       uint32_t *out)
{
	size_t iHead;
	size_t iTail;
	size_t iNext;

	if (buf == NULL || head == NULL || tail == NULL || out == NULL ||
	    cap < 2u) {
		return -1;
	}

	iHead = *head;
	iTail = *tail;
	if (iHead >= cap || iTail >= cap) {
		/* Corrupt cursor — refuse rather than read out of bounds. */
		return -1;
	}

	/* Empty: no elements between head (front) and tail (back). */
	if (iHead == iTail) {
		return -1;
	}

	*out = buf[iHead];

	/* next front/read index modulo cap (wrap without requiring power-of-two). */
	iNext = iHead + 1u;
	if (iNext >= cap) {
		iNext = 0u;
	}
	*head = iNext;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_deque_pop_front(uint32_t *buf, size_t cap, size_t *head,
                             size_t *tail, uint32_t *out)
    __attribute__((alias("gj_u32_deque_pop_front")));
