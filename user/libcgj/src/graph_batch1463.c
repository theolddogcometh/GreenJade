/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1463: classic uint64_t ring push on caller storage.
 *
 * Head/tail live in caller-owned size_t slots (no struct, no heap, no count).
 * Buffer is also caller-owned. Single-threaded / cooperative use (no atomics).
 *
 * Surface (unique; does NOT redefine batch185 gj_u64ring_push (struct API),
 * batch883 gj_spsc_try_write_u64, or batch533 gj_ring_u32_push):
 *   int gj_ring_u64_push(uint64_t *buf, size_t cap, size_t *head, size_t *tail,
 *                        uint64_t v);
 *     — Enqueue one uint64_t at *tail. Returns 0 on success, -1 if full or
 *       NULL args / zero-or-unit capacity / corrupt cursors.
 *   int __gj_ring_u64_push  (alias)
 *   __libcgj_batch1463_marker = "libcgj-batch1463"
 *
 * Ring design (classic head/tail, one empty slot) — same as batch533:
 *   *head — next read index  (0 .. cap-1); not modified by push
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

const char __libcgj_batch1463_marker[] = "libcgj-batch1463";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1463_next(size_t i, size_t cap)
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
 * gj_ring_u64_push — enqueue one uint64_t into a classic head/tail ring.
 *
 * buf:  caller-owned element storage of length cap
 * cap:  number of slots in buf (prefer power-of-two; must be > 1 to push)
 * head: next-read index (not modified by push; used for full detection)
 * tail: next-write index (advanced on success)
 * v:    value to store at buf[*tail]
 *
 * Returns 0 on success; -1 if full, NULL required args, cap < 2, or a
 * cursor is out of range (corrupt). *tail is left untouched on failure.
 * Does not touch *head.
 */
int
gj_ring_u64_push(uint64_t *buf, size_t cap, size_t *head, size_t *tail,
                 uint64_t v)
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

	iNext = b1463_next(iTail, cap);

	/* Full: advancing tail would collide with head (one-empty-slot rule). */
	if (iNext == iHead) {
		return -1;
	}

	buf[iTail] = v;
	*tail = iNext;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ring_u64_push(uint64_t *buf, size_t cap, size_t *head, size_t *tail,
                       uint64_t v)
    __attribute__((alias("gj_ring_u64_push")));
