/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch533: uint32_t ring push on caller storage.
 *
 * Head/tail live in caller-owned size_t slots (no struct, no heap, no count).
 * Buffer is also caller-owned. Single-threaded / cooperative use (no atomics).
 *
 * Surface (unique; does NOT redefine batch117 gj_ring_*, batch281 gj_ring_u8_*,
 * or batch185 gj_u64ring_*):
 *   int gj_ring_u32_push(uint32_t *buf, size_t cap, size_t *head, size_t *tail,
 *                        uint32_t v);
 *     — Enqueue one uint32_t at *tail. Returns 0 on success, -1 if full or
 *       NULL args / zero capacity / corrupt cursors.
 *   int __gj_ring_u32_push  (alias)
 *   __libcgj_batch533_marker = "libcgj-batch533"
 *
 * Ring design (classic head/tail, one empty slot):
 *   *head — next read index  (0 .. cap-1); advanced by a future pop
 *   *tail — next write index (0 .. cap-1); advanced by this push
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
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch533_marker[] = "libcgj-batch533";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_u32_push — enqueue one uint32_t into a classic head/tail ring.
 *
 * buf:  caller-owned element storage of length cap
 * cap:  number of slots in buf (prefer power-of-two; must be > 1 to push)
 * head: next-read index (not modified by push; used for full detection)
 * tail: next-write index (advanced on success)
 * v:    value to store at buf[*tail]
 *
 * Returns 0 on success; -1 if full, NULL required args, cap < 2, or a
 * cursor is out of range (corrupt).
 */
int
gj_ring_u32_push(uint32_t *buf, size_t cap, size_t *head, size_t *tail,
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

int __gj_ring_u32_push(uint32_t *buf, size_t cap, size_t *head, size_t *tail,
                       uint32_t v)
    __attribute__((alias("gj_ring_u32_push")));
