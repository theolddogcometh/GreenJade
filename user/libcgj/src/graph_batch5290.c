/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5290: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5290(void);
 *     - Returns the compile-time graph batch number for this TU (5290).
 *   uint32_t __gj_batch_id_5290  (alias)
 *   __libcgj_batch5290_marker = "libcgj-batch5290"
 *
 * Exclusive continuum CREATE-ONLY (5281-5290: gpu_ring_align_u,
 * gpu_ring_space_u, gpu_ring_used_u, gpu_ring_empty_u, gpu_ring_full_u,
 * gpu_ring_push_u, gpu_ring_pop_u, gpu_fence_seq_u, gpu_fence_signaled_u,
 * batch_id_5290). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5290_marker[] = "libcgj-batch5290";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5290_id(void)
{
	return 5290u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5290 - report this TU's graph batch number.
 *
 * Always returns 5290. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5290(void)
{
	(void)NULL;
	return b5290_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5290(void)
    __attribute__((alias("gj_batch_id_5290")));
