/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5283: occupied slots in a modular GPU ring (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_gpu_ring_used_u(uint32_t head, uint32_t tail, uint32_t size);
 *     - Modular distance from head (consumer) to tail (producer):
 *       used = (tail - head) mod size when cursors are in range; 0 if
 *       size is 0 or a cursor is corrupt. Range is 0 .. size-1 under
 *       the one-empty-slot full/empty rule.
 *   uint32_t __gj_gpu_ring_used_u  (alias)
 *   __libcgj_batch5283_marker = "libcgj-batch5283"
 *
 * Exclusive continuum CREATE-ONLY (5281-5290: gpu_ring_align_u,
 * gpu_ring_space_u, gpu_ring_used_u, gpu_ring_empty_u, gpu_ring_full_u,
 * gpu_ring_push_u, gpu_ring_pop_u, gpu_fence_seq_u, gpu_fence_signaled_u,
 * batch_id_5290). Distinct from gj_ring_avail (batch117) — unique
 * gj_gpu_ring_used_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5283_marker[] = "libcgj-batch5283";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Occupancy of modular ring (indices in [0, size)).
 */
static uint32_t
b5283_used(uint32_t uHead, uint32_t uTail, uint32_t uSize)
{
	if (uSize == 0u || uHead >= uSize || uTail >= uSize) {
		return 0u;
	}
	if (uTail >= uHead) {
		return uTail - uHead;
	}
	return (uSize - uHead) + uTail;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gpu_ring_used_u - occupied slots in a modular GPU ring.
 *
 * head: next-pop index (consumer)
 * tail: next-push index (producer)
 * size: ring capacity in slots
 *
 * Returns modular (tail - head), or 0 when size is 0 or a cursor is
 * out of range. Does not call libc. No parent wires.
 */
uint32_t
gj_gpu_ring_used_u(uint32_t uHead, uint32_t uTail, uint32_t uSize)
{
	(void)NULL;
	return b5283_used(uHead, uTail, uSize);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gpu_ring_used_u(uint32_t uHead, uint32_t uTail, uint32_t uSize)
    __attribute__((alias("gj_gpu_ring_used_u")));
