/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1469: uint16_t classic ring deque pop-front.
 *
 * Head/tail live in caller-owned size_t slots (no struct, no heap, no count).
 * Buffer is also caller-owned. Single-threaded / cooperative use (no atomics).
 * Pairs with batch1467/1468 (same classic head/tail ring).
 *
 * Surface (unique; does NOT redefine batch705 gj_u32_deque_pop_front —
 * avoid multi-def):
 *   int gj_deque_u16_pop_front(uint16_t *buf, size_t cap, size_t *head,
 *                              size_t *tail, uint16_t *out);
 *     — Pop one uint16_t from the front (*head) into *out. Returns 0 on
 *       success, -1 if empty or NULL args / zero-or-unit capacity /
 *       corrupt cursors.
 *   int __gj_deque_u16_pop_front  (alias)
 *   __libcgj_batch1469_marker = "libcgj-batch1469"
 *
 * Ring design (classic head/tail, one empty slot) — same as batch1467/1468:
 *   *head — next front/read index  (0 .. cap-1); advanced by this pop
 *   *tail — next back/write index  (0 .. cap-1); not modified by pop
 *   empty when *head == *tail
 *   usable capacity is (cap - 1)
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1469_marker[] = "libcgj-batch1469";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1469_next(size_t i, size_t cap)
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
 * gj_deque_u16_pop_front — remove one uint16_t from the front of a classic
 * head/tail ring deque.
 *
 * buf:  caller-owned element storage of length cap
 * cap:  number of slots in buf (prefer power-of-two; must be > 1 to pop)
 * head: next-front/read index (advanced on success)
 * tail: next-back/write index (not modified by pop; used for empty detection)
 * out:  destination for the popped value (must be non-NULL)
 *
 * Returns 0 on success; -1 if empty, NULL required args, cap < 2, or a
 * cursor is out of range (corrupt). *head and *out are left untouched on
 * failure. Does not touch *tail.
 */
int
gj_deque_u16_pop_front(uint16_t *buf, size_t cap, size_t *head, size_t *tail,
                       uint16_t *out)
{
	size_t iHead;
	size_t iTail;

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
	*head = b1469_next(iHead, cap);
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_deque_u16_pop_front(uint16_t *buf, size_t cap, size_t *head,
                             size_t *tail, uint16_t *out)
    __attribute__((alias("gj_deque_u16_pop_front")));
