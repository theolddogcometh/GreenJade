/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5281: GPU ring buffer alignment quantum (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_gpu_ring_align_u(void);
 *     - Returns the GPU command/data ring alignment quantum (256 bytes).
 *       Common hardware submission and BO base alignment for ring slots.
 *   uint32_t __gj_gpu_ring_align_u  (alias)
 *   __libcgj_batch5281_marker = "libcgj-batch5281"
 *
 * Exclusive continuum CREATE-ONLY (5281-5290: gpu_ring_align_u,
 * gpu_ring_space_u, gpu_ring_used_u, gpu_ring_empty_u, gpu_ring_full_u,
 * gpu_ring_push_u, gpu_ring_pop_u, gpu_fence_seq_u, gpu_fence_signaled_u,
 * batch_id_5290). Unique gj_gpu_ring_align_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5281_marker[] = "libcgj-batch5281";

/* GPU ring alignment quantum (bytes). */
#define B5281_RING_ALIGN  256u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5281_align(void)
{
	return B5281_RING_ALIGN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gpu_ring_align_u - report the GPU ring alignment quantum.
 *
 * Always returns 256. Use for ring BO base and slot stride alignment
 * checks in freestanding GPU submission helpers. No parent wires.
 */
uint32_t
gj_gpu_ring_align_u(void)
{
	(void)NULL;
	return b5281_align();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gpu_ring_align_u(void)
    __attribute__((alias("gj_gpu_ring_align_u")));
