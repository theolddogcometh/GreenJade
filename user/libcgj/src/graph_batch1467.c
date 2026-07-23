/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1467: uint16_t classic ring deque push-front.
 *
 * Head/tail live in caller-owned size_t slots (no struct, no heap, no count).
 * Buffer is also caller-owned. Single-threaded / cooperative use (no atomics).
 *
 * Surface (unique; does NOT redefine batch703 gj_u32_deque_push_front or
 * batch438 gj_deque_u32_push_front — avoid multi-def):
 *   int gj_deque_u16_push_front(uint16_t *buf, size_t cap, size_t *head,
 *                               size_t *tail, uint16_t v);
 *     — Insert one uint16_t at the front (*head side). Returns 0 on
 *       success, -1 if full or NULL args / zero-or-unit capacity /
 *       corrupt cursors.
 *   int __gj_deque_u16_push_front  (alias)
 *   __libcgj_batch1467_marker = "libcgj-batch1467"
 *
 * Ring design (classic head/tail, one empty slot) — same as batch703:
 *   *head — next read / front index  (0 .. cap-1); retreated by this push
 *   *tail — next back-write index    (0 .. cap-1); not modified
 *   empty when *head == *tail
 *   full  when retreating head would land on *tail
 *   usable capacity is (cap - 1)
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1467_marker[] = "libcgj-batch1467";

/* ---- freestanding helpers ---------------------------------------------- */

/* Retreat index by one with modular wrap (any cap > 0). */
static size_t
b1467_prev(size_t i, size_t cap)
{
	if (i == 0u) {
		return cap - 1u;
	}
	return i - 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deque_u16_push_front — insert one uint16_t at the front of a classic
 * head/tail ring (one empty slot).
 *
 * buf:  caller-owned element storage of length cap
 * cap:  number of slots in buf (prefer power-of-two; must be > 1 to push)
 * head: front / next-read index (retreated on success; store at new head)
 * tail: next back-write index (not modified; used for full detection)
 * v:    value to store at the new front slot
 *
 * Returns 0 on success; -1 if full, NULL required args, cap < 2, or a
 * cursor is out of range (corrupt). *head is left untouched on failure.
 * Does not touch *tail.
 */
int
gj_deque_u16_push_front(uint16_t *buf, size_t cap, size_t *head, size_t *tail,
                        uint16_t v)
{
	size_t iHead;
	size_t iTail;
	size_t iNew;

	if (buf == NULL || head == NULL || tail == NULL || cap < 2u) {
		return -1;
	}

	iHead = *head;
	iTail = *tail;
	if (iHead >= cap || iTail >= cap) {
		/* Corrupt cursor — refuse rather than write out of bounds. */
		return -1;
	}

	iNew = b1467_prev(iHead, cap);

	/* Full: retreating head would collide with tail (one-empty-slot rule). */
	if (iNew == iTail) {
		return -1;
	}

	buf[iNew] = v;
	*head = iNew;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_deque_u16_push_front(uint16_t *buf, size_t cap, size_t *head,
                              size_t *tail, uint16_t v)
    __attribute__((alias("gj_deque_u16_push_front")));
