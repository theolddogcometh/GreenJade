/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4358: ring buffer empty predicate.
 *
 * Surface (unique symbols):
 *   size_t gj_ring_empty_u(size_t head, size_t tail);
 *     - Returns 1 when head == tail (classic empty ring), else 0.
 *       Capacity is not required: empty is purely cursor equality under
 *       the leave-one-empty / head-tail scheme.
 *   size_t __gj_ring_empty_u  (alias)
 *   __libcgj_batch4358_marker = "libcgj-batch4358"
 *
 * Exclusive continuum CREATE-ONLY (4351-4360: ring_next_u, ring_prev_u,
 * ring_add_u, ring_distance_u, ring_used_u, ring_free_u, ring_full_u,
 * ring_empty_u, ring_index_u, batch_id_4360). Pure index math only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4358_marker[] = "libcgj-batch4358";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4358_empty(size_t nHead, size_t nTail)
{
	if (nHead == nTail) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_empty_u - empty predicate for a head/tail ring.
 *
 * head: next write index
 * tail: next read index
 *
 * Returns 1 when head == tail, else 0.
 * Self-contained; no parent wires.
 */
size_t
gj_ring_empty_u(size_t nHead, size_t nTail)
{
	(void)NULL;
	return b4358_empty(nHead, nTail);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_ring_empty_u(size_t nHead, size_t nTail)
    __attribute__((alias("gj_ring_empty_u")));
