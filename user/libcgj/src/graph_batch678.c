/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch678: uint32_t classic ring queue pop.
 *
 * Head/tail live in caller-owned size_t slots (no struct, no heap, no count).
 * Buffer is also caller-owned. Single-threaded / cooperative use (no atomics).
 * Pairs with batch677 gj_u32_queue_push (classic head/tail, one empty slot).
 *
 * Surface (unique; does NOT redefine batch533 gj_ring_u32_pop,
 * batch437 gj_queue_u32_pop, batch281 gj_ring_u8_*, batch175 gj_wq_*, or
 * batch185 gj_u64ring_* — avoid multi-def):
 *   int gj_u32_queue_pop(uint32_t *buf, size_t cap, size_t *head,
 *                        size_t *tail, uint32_t *out);
 *     — Dequeue one uint32_t from *head into *out. Returns 0 (OK) on
 *       success, -1 (FAIL) if empty or NULL args / zero-or-unit capacity /
 *       corrupt cursors.
 *   int __gj_u32_queue_pop  (alias)
 *   __libcgj_batch678_marker = "libcgj-batch678"
 *
 * Ring design (classic head/tail, one empty slot) — same as batch677:
 *   *head — next read index  (0 .. cap-1); advanced by this pop
 *   *tail — next write index (0 .. cap-1); advanced by push (batch677)
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

const char __libcgj_batch678_marker[] = "libcgj-batch678";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_queue_pop — dequeue one uint32_t from a classic head/tail ring.
 *
 * buf:  caller-owned element storage of length cap
 * cap:  number of slots in buf (prefer power-of-two; must be > 1 to pop)
 * head: next-read index (advanced on success)
 * tail: next-write index (not modified by pop; used for empty detection)
 * out:  destination for the dequeued value (must be non-NULL)
 *
 * Returns 0 (OK) on success; -1 (FAIL) if empty, NULL required args, cap < 2,
 * or a cursor is out of range (corrupt). *head and *out are left untouched on
 * failure. Does not touch *tail.
 */
int
gj_u32_queue_pop(uint32_t *buf, size_t cap, size_t *head, size_t *tail,
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

	/* Empty: no elements between head and tail. */
	if (iHead == iTail) {
		return -1;
	}

	*out = buf[iHead];

	/* next read index modulo cap (wrap without requiring power-of-two). */
	iNext = iHead + 1u;
	if (iNext >= cap) {
		iNext = 0u;
	}
	*head = iNext;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_queue_pop(uint32_t *buf, size_t cap, size_t *head, size_t *tail,
                       uint32_t *out)
    __attribute__((alias("gj_u32_queue_pop")));
