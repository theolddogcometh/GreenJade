/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5286: advance GPU ring tail after push (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_gpu_ring_push_u(uint32_t tail, uint32_t size);
 *     - Return the next producer index after a successful push:
 *       (tail + 1) mod size. Returns 0 when size is 0. Pure cursor
 *       math; caller owns the payload store and full checks.
 *   uint32_t __gj_gpu_ring_push_u  (alias)
 *   __libcgj_batch5286_marker = "libcgj-batch5286"
 *
 * Exclusive continuum CREATE-ONLY (5281-5290: gpu_ring_align_u,
 * gpu_ring_space_u, gpu_ring_used_u, gpu_ring_empty_u, gpu_ring_full_u,
 * gpu_ring_push_u, gpu_ring_pop_u, gpu_fence_seq_u, gpu_fence_signaled_u,
 * batch_id_5290). Distinct from gj_ring_u32_push (batch533) — unique
 * gj_gpu_ring_push_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5286_marker[] = "libcgj-batch5286";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Next producer index modulo size (wrap without requiring power-of-two).
 */
static uint32_t
b5286_push(uint32_t uTail, uint32_t uSize)
{
	uint32_t uNext;

	if (uSize == 0u) {
		return 0u;
	}
	/* Normalize tail into [0, size) before advance. */
	if (uTail >= uSize) {
		uTail = uTail % uSize;
	}
	uNext = uTail + 1u;
	if (uNext >= uSize) {
		uNext = 0u;
	}
	return uNext;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gpu_ring_push_u - advance the GPU ring producer cursor.
 *
 * tail: current next-push index
 * size: ring capacity in slots
 *
 * Returns (tail + 1) mod size, or 0 when size is 0. Does not write
 * payload or test full — pair with gj_gpu_ring_full_u /
 * gj_gpu_ring_space_u. Does not call libc. No parent wires.
 */
uint32_t
gj_gpu_ring_push_u(uint32_t uTail, uint32_t uSize)
{
	(void)NULL;
	return b5286_push(uTail, uSize);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gpu_ring_push_u(uint32_t uTail, uint32_t uSize)
    __attribute__((alias("gj_gpu_ring_push_u")));
