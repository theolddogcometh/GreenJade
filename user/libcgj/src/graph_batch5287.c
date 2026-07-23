/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5287: advance GPU ring head after pop (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_gpu_ring_pop_u(uint32_t head, uint32_t size);
 *     - Return the next consumer index after a successful pop:
 *       (head + 1) mod size. Returns 0 when size is 0. Pure cursor
 *       math; caller owns the payload store and empty checks.
 *   uint32_t __gj_gpu_ring_pop_u  (alias)
 *   __libcgj_batch5287_marker = "libcgj-batch5287"
 *
 * Exclusive continuum CREATE-ONLY (5281-5290: gpu_ring_align_u,
 * gpu_ring_space_u, gpu_ring_used_u, gpu_ring_empty_u, gpu_ring_full_u,
 * gpu_ring_push_u, gpu_ring_pop_u, gpu_fence_seq_u, gpu_fence_signaled_u,
 * batch_id_5290). Unique gj_gpu_ring_pop_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5287_marker[] = "libcgj-batch5287";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Next consumer index modulo size (wrap without requiring power-of-two).
 */
static uint32_t
b5287_pop(uint32_t uHead, uint32_t uSize)
{
	uint32_t uNext;

	if (uSize == 0u) {
		return 0u;
	}
	/* Normalize head into [0, size) before advance. */
	if (uHead >= uSize) {
		uHead = uHead % uSize;
	}
	uNext = uHead + 1u;
	if (uNext >= uSize) {
		uNext = 0u;
	}
	return uNext;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gpu_ring_pop_u - advance the GPU ring consumer cursor.
 *
 * head: current next-pop index
 * size: ring capacity in slots
 *
 * Returns (head + 1) mod size, or 0 when size is 0. Does not read
 * payload or test empty — pair with gj_gpu_ring_empty_u /
 * gj_gpu_ring_used_u. Does not call libc. No parent wires.
 */
uint32_t
gj_gpu_ring_pop_u(uint32_t uHead, uint32_t uSize)
{
	(void)NULL;
	return b5287_pop(uHead, uSize);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gpu_ring_pop_u(uint32_t uHead, uint32_t uSize)
    __attribute__((alias("gj_gpu_ring_pop_u")));
