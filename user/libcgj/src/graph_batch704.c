/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch704: uint32_t classic ring deque push-back.
 *
 * Head/tail live in caller-owned size_t slots (no struct, no heap, no count).
 * Buffer is also caller-owned. Single-threaded / cooperative use (no atomics).
 * Pairs with batch703 gj_u32_deque_push_front (same classic head/tail ring).
 *
 * Surface (unique; does NOT redefine batch438 gj_deque_u32_push_back
 * (count-based ring), batch677 gj_u32_queue_push, batch533 gj_ring_u32_*,
 * batch703 gj_u32_deque_push_front, or batch281 gj_ring_u8_* — avoid multi-def):
 *   int gj_u32_deque_push_back(uint32_t *buf, size_t cap, size_t *head,
 *                              size_t *tail, uint32_t v);
 *     — Insert one uint32_t at the back (*tail side). Returns 0 (OK) on
 *       success, -1 (FAIL) if full or NULL args / zero-or-unit capacity /
 *       corrupt cursors.
 *   int __gj_u32_deque_push_back  (alias)
 *   __libcgj_batch704_marker = "libcgj-batch704"
 *
 * Ring design (classic head/tail, one empty slot) — same as batch703:
 *   *head — next read / front index  (0 .. cap-1); not modified by push_back
 *   *tail — next back-write index    (0 .. cap-1); advanced by this push
 *   empty when *head == *tail
 *   full  when (*tail + 1) mod cap == *head
 *   used  = (*tail - *head) mod cap   (0 .. cap-1)
 *   usable capacity is (cap - 1); one slot is reserved so full and empty
 *   remain distinguishable without a separate count field.
 *
 * Indices use modular wrap. A power-of-two cap is preferred (cheap mask
 * equivalent, natural alignment) but any cap > 1 is accepted. cap == 0 or
 * cap == 1 cannot hold an element under this design.
 *
 * Init (caller): *head = *tail = 0; buf[0..cap-1] is caller-owned storage.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch704_marker[] = "libcgj-batch704";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_deque_push_back — insert one uint32_t at the back of a classic
 * head/tail ring (one empty slot). Same ring as batch703.
 *
 * buf:  caller-owned element storage of length cap
 * cap:  number of slots in buf (prefer power-of-two; must be > 1 to push)
 * head: front / next-read index (not modified; used for full detection)
 * tail: next back-write index (advanced on success; store lands at old tail)
 * v:    value to store at buf[*tail]
 *
 * Returns 0 (OK) on success; -1 (FAIL) if full, NULL required args, cap < 2,
 * or a cursor is out of range (corrupt). *tail is left untouched on failure.
 * Does not touch *head.
 */
int
gj_u32_deque_push_back(uint32_t *buf, size_t cap, size_t *head, size_t *tail,
                       uint32_t v)
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

	/* next write index modulo cap (wrap without requiring power-of-two). */
	iNext = iTail + 1u;
	if (iNext >= cap) {
		iNext = 0u;
	}

	/* Full: advancing tail would collide with head (one-empty-slot rule). */
	if (iNext == iHead) {
		return -1;
	}

	buf[iTail] = v;
	*tail = iNext;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_deque_push_back(uint32_t *buf, size_t cap, size_t *head,
                             size_t *tail, uint32_t v)
    __attribute__((alias("gj_u32_deque_push_back")));
