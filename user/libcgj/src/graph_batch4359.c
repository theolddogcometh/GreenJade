/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4359: ring buffer base+offset modular index.
 *
 * Surface (unique symbols):
 *   size_t gj_ring_index_u(size_t base, size_t i, size_t cap);
 *     - Absolute slot for logical offset i from base:
 *       (base + i) % cap. When cap == 0, return 0.
 *   size_t __gj_ring_index_u  (alias)
 *   __libcgj_batch4359_marker = "libcgj-batch4359"
 *
 * Exclusive continuum CREATE-ONLY (4351-4360: ring_next_u, ring_prev_u,
 * ring_add_u, ring_distance_u, ring_used_u, ring_free_u, ring_full_u,
 * ring_empty_u, ring_index_u, batch_id_4360). Distinct from gj_ring_add_u
 * (batch4353) only by naming/role (absolute map vs advance); same modular
 * add math, unique symbols. No multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4359_marker[] = "libcgj-batch4359";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4359_index(size_t nBase, size_t nI, size_t nCap)
{
	size_t nA;
	size_t nB;

	if (nCap == 0u) {
		return 0u;
	}
	/* Reduce each operand mod cap before add to avoid size_t wrap. */
	nA = nBase % nCap;
	nB = nI % nCap;
	return (nA + nB) % nCap;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_index_u - map logical offset i from base into a ring slot.
 *
 * base: base/start index
 * i:    logical offset (may exceed cap; reduced mod cap)
 * cap:  ring capacity in slots
 *
 * Returns (base + i) % cap, or 0 when cap is 0.
 * Self-contained; no parent wires.
 */
size_t
gj_ring_index_u(size_t nBase, size_t nI, size_t nCap)
{
	(void)NULL;
	return b4359_index(nBase, nI, nCap);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_ring_index_u(size_t nBase, size_t nI, size_t nCap)
    __attribute__((alias("gj_ring_index_u")));
