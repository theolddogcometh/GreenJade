/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch706: uint32_t classic ring deque pop-back.
 *
 * Head/tail live in caller-owned size_t slots (no struct, no heap, no count).
 * Buffer is also caller-owned. Single-threaded / cooperative use (no atomics).
 * Classic head/tail, one empty slot — same ring layout as batch677/678 queue.
 *
 * Surface (unique; does NOT redefine batch678 gj_u32_queue_pop,
 * batch534 gj_ring_u32_pop, batch437 gj_queue_u32_pop, batch438
 * gj_deque_u32_push_*, or batch281 gj_ring_u8_* — avoid multi-def):
 *   int gj_u32_deque_pop_back(uint32_t *buf, size_t cap, size_t *head,
 *                             size_t *tail, uint32_t *out);
 *     — Remove one uint32_t from the back (slot before *tail) into *out.
 *       Returns 0 (OK) on success, -1 (FAIL) if empty or NULL args /
 *       zero-or-unit capacity / corrupt cursors.
 *   int __gj_u32_deque_pop_back  (alias)
 *   __libcgj_batch706_marker = "libcgj-batch706"
 *
 * Ring design (classic head/tail, one empty slot) — same as batch677/678:
 *   *head — front / next-read index (0 .. cap-1); not modified by pop_back
 *   *tail — next write index at the back (0 .. cap-1); retreated by this pop
 *   empty when *head == *tail
 *   full  when (*tail + 1) mod cap == *head
 *   used  = (*tail - *head) mod cap   (0 .. cap-1)
 *   usable capacity is (cap - 1); one slot is reserved so full and empty
 *   remain distinguishable without a separate count field.
 *
 * Back element lives at (*tail - 1) mod cap (the last slot written by a
 * back-push). Pop-back retreats *tail to that index after copying out.
 *
 * Indices use modular wrap. A power-of-two cap is preferred but any cap > 1
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

const char __libcgj_batch706_marker[] = "libcgj-batch706";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_deque_pop_back — remove one uint32_t from the back of a classic
 * head/tail ring deque.
 *
 * buf:  caller-owned element storage of length cap
 * cap:  number of slots in buf (prefer power-of-two; must be > 1 to pop)
 * head: front / next-read index (not modified; used for empty detection)
 * tail: next-write index at the back (retreated on success)
 * out:  destination for the removed value (must be non-NULL)
 *
 * Returns 0 (OK) on success; -1 (FAIL) if empty, NULL required args, cap < 2,
 * or a cursor is out of range (corrupt). *tail and *out are left untouched on
 * failure. Does not touch *head.
 */
int
gj_u32_deque_pop_back(uint32_t *buf, size_t cap, size_t *head, size_t *tail,
                      uint32_t *out)
{
	size_t iHead;
	size_t iTail;
	size_t iBack;

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

	/* Empty: no elements between head and tail. */
	if (iHead == iTail) {
		return -1;
	}

	/* Back slot is one before the next-write index (mod cap). */
	if (iTail == 0u) {
		iBack = cap - 1u;
	} else {
		iBack = iTail - 1u;
	}

	*out = buf[iBack];
	*tail = iBack;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_deque_pop_back(uint32_t *buf, size_t cap, size_t *head,
                            size_t *tail, uint32_t *out)
    __attribute__((alias("gj_u32_deque_pop_back")));
