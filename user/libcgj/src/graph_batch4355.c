/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4355: ring buffer used-slot count.
 *
 * Surface (unique symbols):
 *   size_t gj_ring_used_u(size_t head, size_t tail, size_t cap);
 *     - Occupied slots for a classic head/tail ring (head = next write,
 *       tail = next read): forward distance tail → head =
 *       (head - tail + cap) % cap. When cap == 0, return 0.
 *   size_t __gj_ring_used_u  (alias)
 *   __libcgj_batch4355_marker = "libcgj-batch4355"
 *
 * Exclusive continuum CREATE-ONLY (4351-4360: ring_next_u, ring_prev_u,
 * ring_add_u, ring_distance_u, ring_used_u, ring_free_u, ring_full_u,
 * ring_empty_u, ring_index_u, batch_id_4360). Distinct from gj_ring_avail
 * (batch117) — pure index math only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4355_marker[] = "libcgj-batch4355";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4355_used(size_t nHead, size_t nTail, size_t nCap)
{
	size_t nH;
	size_t nT;

	if (nCap == 0u) {
		return 0u;
	}
	nH = nHead % nCap;
	nT = nTail % nCap;
	return (nH + nCap - nT) % nCap;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_used_u - number of occupied slots in a head/tail ring.
 *
 * head: next write index
 * tail: next read index
 * cap:  ring capacity in slots
 *
 * Returns (head - tail + cap) % cap, or 0 when cap is 0.
 * Self-contained; no parent wires.
 */
size_t
gj_ring_used_u(size_t nHead, size_t nTail, size_t nCap)
{
	(void)NULL;
	return b4355_used(nHead, nTail, nCap);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_ring_used_u(size_t nHead, size_t nTail, size_t nCap)
    __attribute__((alias("gj_ring_used_u")));
