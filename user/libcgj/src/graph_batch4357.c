/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4357: ring buffer full predicate (leave-one-empty).
 *
 * Surface (unique symbols):
 *   size_t gj_ring_full_u(size_t head, size_t tail, size_t cap);
 *     - Returns 1 when used == cap - 1 (leave-one-empty full watermark),
 *       else 0. When cap == 0, return 0 (degenerate; never full).
 *   size_t __gj_ring_full_u  (alias)
 *   __libcgj_batch4357_marker = "libcgj-batch4357"
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

const char __libcgj_batch4357_marker[] = "libcgj-batch4357";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4357_used(size_t nHead, size_t nTail, size_t nCap)
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

static size_t
b4357_full(size_t nHead, size_t nTail, size_t nCap)
{
	if (nCap == 0u) {
		return 0u;
	}
	if (b4357_used(nHead, nTail, nCap) == (nCap - 1u)) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_full_u - leave-one-empty full predicate for a head/tail ring.
 *
 * head: next write index
 * tail: next read index
 * cap:  ring capacity in slots
 *
 * Returns 1 if used == cap - 1, else 0. Cap 0 yields 0.
 * Self-contained; no parent wires.
 */
size_t
gj_ring_full_u(size_t nHead, size_t nTail, size_t nCap)
{
	(void)NULL;
	return b4357_full(nHead, nTail, nCap);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_ring_full_u(size_t nHead, size_t nTail, size_t nCap)
    __attribute__((alias("gj_ring_full_u")));
