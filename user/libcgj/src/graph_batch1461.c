/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1461: uint32_t ring force-push (overwrite oldest).
 *
 * Head/tail live in caller-owned size_t slots (no struct, no heap, no count).
 * Buffer is also caller-owned. Single-threaded / cooperative use (no atomics).
 *
 * Surface (unique; does NOT redefine batch533 gj_ring_u32_push which fails
 * when full — this force variant always enqueues by dropping the oldest):
 *   int gj_ring_u32_push_force(uint32_t *buf, size_t cap, size_t *head,
 *                              size_t *tail, uint32_t v);
 *     — Enqueue v at *tail. If the classic one-empty-slot ring is full,
 *       advance *head (discard oldest) then write. Returns 0 on success,
 *       -1 if NULL args / cap < 2 / corrupt cursors.
 *   int __gj_ring_u32_push_force  (alias)
 *   __libcgj_batch1461_marker = "libcgj-batch1461"
 *
 * Ring design (classic head/tail, one empty slot) — same as batch533/534:
 *   *head — next read index  (0 .. cap-1); may advance when overwriting
 *   *tail — next write index (0 .. cap-1); advanced by this push
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

const char __libcgj_batch1461_marker[] = "libcgj-batch1461";

/* ---- freestanding helpers ---------------------------------------------- */

/* Advance index by one with modular wrap (any cap > 0). */
static size_t
b1461_next(size_t i, size_t cap)
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
 * gj_ring_u32_push_force — enqueue one uint32_t, overwriting oldest if full.
 *
 * buf:  caller-owned element storage of length cap
 * cap:  number of slots in buf (must be > 1)
 * head: next-read index (advanced when dropping oldest under full ring)
 * tail: next-write index (advanced on success)
 * v:    value to store at buf[*tail]
 *
 * Returns 0 on success; -1 if NULL required args, cap < 2, or corrupt
 * cursors. On failure *head and *tail are left untouched.
 */
int
gj_ring_u32_push_force(uint32_t *buf, size_t cap, size_t *head, size_t *tail,
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

	iNext = b1461_next(iTail, cap);

	/* Full: drop oldest by advancing head, then write. */
	if (iNext == iHead) {
		iHead = b1461_next(iHead, cap);
		*head = iHead;
	}

	buf[iTail] = v;
	*tail = iNext;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ring_u32_push_force(uint32_t *buf, size_t cap, size_t *head,
                             size_t *tail, uint32_t v)
    __attribute__((alias("gj_ring_u32_push_force")));
