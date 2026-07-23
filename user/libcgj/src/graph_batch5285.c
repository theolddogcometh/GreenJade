/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5285: GPU ring full predicate (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_gpu_ring_full_u(uint32_t head, uint32_t tail, uint32_t size);
 *     - Returns 1 when advancing tail would collide with head under the
 *       one-empty-slot modular ring rule: ((tail + 1) mod size) == head.
 *       Also 1 when size < 2 (no usable slot). Else 0.
 *   uint32_t __gj_gpu_ring_full_u  (alias)
 *   __libcgj_batch5285_marker = "libcgj-batch5285"
 *
 * Exclusive continuum CREATE-ONLY (5281-5290: gpu_ring_align_u,
 * gpu_ring_space_u, gpu_ring_used_u, gpu_ring_empty_u, gpu_ring_full_u,
 * gpu_ring_push_u, gpu_ring_pop_u, gpu_fence_seq_u, gpu_fence_signaled_u,
 * batch_id_5290). Unique gj_gpu_ring_full_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5285_marker[] = "libcgj-batch5285";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Full when next write index would equal head (one empty slot reserved).
 * size < 2 cannot hold an element. Corrupt cursors treated as full.
 */
static uint32_t
b5285_full(uint32_t uHead, uint32_t uTail, uint32_t uSize)
{
	uint32_t uNext;

	if (uSize < 2u) {
		return 1u;
	}
	if (uHead >= uSize || uTail >= uSize) {
		return 1u;
	}
	uNext = uTail + 1u;
	if (uNext >= uSize) {
		uNext = 0u;
	}
	if (uNext == uHead) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gpu_ring_full_u - test whether a modular GPU ring is full.
 *
 * head: next-pop index (consumer)
 * tail: next-push index (producer)
 * size: ring capacity in slots
 *
 * Returns 1 if no push slot remains (or size/cursors unusable), else 0.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_gpu_ring_full_u(uint32_t uHead, uint32_t uTail, uint32_t uSize)
{
	(void)NULL;
	return b5285_full(uHead, uTail, uSize);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gpu_ring_full_u(uint32_t uHead, uint32_t uTail, uint32_t uSize)
    __attribute__((alias("gj_gpu_ring_full_u")));
