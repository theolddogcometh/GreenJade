/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1468: uint16_t classic ring deque push-back.
 *
 * Head/tail live in caller-owned size_t slots (no struct, no heap, no count).
 * Buffer is also caller-owned. Single-threaded / cooperative use (no atomics).
 * Pairs with batch1467 gj_deque_u16_push_front (same classic head/tail ring).
 *
 * Surface (unique; does NOT redefine batch704 gj_u32_deque_push_back or
 * batch438 gj_deque_u32_push_back — avoid multi-def):
 *   int gj_deque_u16_push_back(uint16_t *buf, size_t cap, size_t *head,
 *                              size_t *tail, uint16_t v);
 *     — Insert one uint16_t at the back (*tail side). Returns 0 on
 *       success, -1 if full or NULL args / zero-or-unit capacity /
 *       corrupt cursors.
 *   int __gj_deque_u16_push_back  (alias)
 *   __libcgj_batch1468_marker = "libcgj-batch1468"
 *
 * Ring design (classic head/tail, one empty slot) — same as batch1467/704:
 *   *head — next read / front index  (0 .. cap-1); not modified by push_back
 *   *tail — next back-write index    (0 .. cap-1); advanced by this push
 *   empty when *head == *tail
 *   full  when (*tail + 1) mod cap == *head
 *   usable capacity is (cap - 1)
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1468_marker[] = "libcgj-batch1468";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1468_next(size_t i, size_t cap)
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
 * gj_deque_u16_push_back — insert one uint16_t at the back of a classic
 * head/tail ring (one empty slot). Same ring as batch1467.
 *
 * buf:  caller-owned element storage of length cap
 * cap:  number of slots in buf (prefer power-of-two; must be > 1 to push)
 * head: front / next-read index (not modified; used for full detection)
 * tail: next back-write index (advanced on success; store lands at old tail)
 * v:    value to store at buf[*tail]
 *
 * Returns 0 on success; -1 if full, NULL required args, cap < 2, or a
 * cursor is out of range (corrupt). *tail is left untouched on failure.
 * Does not touch *head.
 */
int
gj_deque_u16_push_back(uint16_t *buf, size_t cap, size_t *head, size_t *tail,
                       uint16_t v)
{
	size_t iHead;
	size_t iTail;
	size_t iNext;

	if (buf == NULL || head == NULL || tail == NULL || cap < 2u) {
		return -1;
	}

	iHead = *head;
	iTail = *tail;
	if (iHead >= cap || iTail >= cap) {
		/* Corrupt cursor — refuse rather than write out of bounds. */
		return -1;
	}

	iNext = b1468_next(iTail, cap);

	/* Full: advancing tail would collide with head (one-empty-slot rule). */
	if (iNext == iHead) {
		return -1;
	}

	buf[iTail] = v;
	*tail = iNext;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_deque_u16_push_back(uint16_t *buf, size_t cap, size_t *head,
                             size_t *tail, uint16_t v)
    __attribute__((alias("gj_deque_u16_push_back")));
