/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5282: free slots in a modular GPU ring (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_gpu_ring_space_u(uint32_t head, uint32_t tail, uint32_t size);
 *     - Free push slots under classic head/tail modular ring with one
 *       empty slot reserved so full and empty remain distinguishable.
 *       space = (size - 1) - used when size >= 2; else 0.
 *   uint32_t __gj_gpu_ring_space_u  (alias)
 *   __libcgj_batch5282_marker = "libcgj-batch5282"
 *
 * Exclusive continuum CREATE-ONLY (5281-5290: gpu_ring_align_u,
 * gpu_ring_space_u, gpu_ring_used_u, gpu_ring_empty_u, gpu_ring_full_u,
 * gpu_ring_push_u, gpu_ring_pop_u, gpu_fence_seq_u, gpu_fence_signaled_u,
 * batch_id_5290). Distinct from gj_ring_space (batch117) — unique
 * gj_gpu_ring_space_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5282_marker[] = "libcgj-batch5282";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Occupancy of modular ring (indices in [0, size)). Returns 0 if size
 * is 0 or cursors are out of range.
 */
static uint32_t
b5282_used(uint32_t uHead, uint32_t uTail, uint32_t uSize)
{
	if (uSize == 0u || uHead >= uSize || uTail >= uSize) {
		return 0u;
	}
	if (uTail >= uHead) {
		return uTail - uHead;
	}
	return (uSize - uHead) + uTail;
}

/*
 * Free push slots with one-empty-slot full/empty rule.
 */
static uint32_t
b5282_space(uint32_t uHead, uint32_t uTail, uint32_t uSize)
{
	uint32_t uUsed;
	uint32_t uMax;

	if (uSize < 2u) {
		return 0u;
	}
	uUsed = b5282_used(uHead, uTail, uSize);
	uMax = uSize - 1u;
	if (uUsed >= uMax) {
		return 0u;
	}
	return uMax - uUsed;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gpu_ring_space_u - free push slots in a modular GPU ring.
 *
 * head: next-pop index (consumer)
 * tail: next-push index (producer)
 * size: ring capacity in slots (indices wrap at size)
 *
 * Returns available push slots under the one-empty-slot rule, or 0 when
 * size < 2 or cursors are corrupt. Does not call libc. No parent wires.
 */
uint32_t
gj_gpu_ring_space_u(uint32_t uHead, uint32_t uTail, uint32_t uSize)
{
	(void)NULL;
	return b5282_space(uHead, uTail, uSize);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gpu_ring_space_u(uint32_t uHead, uint32_t uTail, uint32_t uSize)
    __attribute__((alias("gj_gpu_ring_space_u")));
