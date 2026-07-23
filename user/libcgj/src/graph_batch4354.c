/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4354: ring buffer forward modular distance.
 *
 * Surface (unique symbols):
 *   size_t gj_ring_distance_u(size_t from, size_t to, size_t cap);
 *     - Forward distance from → to on a ring of capacity cap:
 *       (to - from + cap) % cap. When cap == 0, return 0.
 *   size_t __gj_ring_distance_u  (alias)
 *   __libcgj_batch4354_marker = "libcgj-batch4354"
 *
 * Exclusive continuum CREATE-ONLY (4351-4360: ring_next_u, ring_prev_u,
 * ring_add_u, ring_distance_u, ring_used_u, ring_free_u, ring_full_u,
 * ring_empty_u, ring_index_u, batch_id_4360). Distinct from gj_ring_*
 * struct/cursor APIs — pure index math only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4354_marker[] = "libcgj-batch4354";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4354_distance(size_t nFrom, size_t nTo, size_t nCap)
{
	size_t nA;
	size_t nB;

	if (nCap == 0u) {
		return 0u;
	}
	nA = nFrom % nCap;
	nB = nTo % nCap;
	return (nB + nCap - nA) % nCap;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_distance_u - forward distance from → to under capacity cap.
 *
 * from: start slot
 * to:   end slot
 * cap:  ring capacity in slots
 *
 * Returns (to - from + cap) % cap, or 0 when cap is 0. Equal endpoints
 * yield distance 0.
 * Self-contained; no parent wires.
 */
size_t
gj_ring_distance_u(size_t nFrom, size_t nTo, size_t nCap)
{
	(void)NULL;
	return b4354_distance(nFrom, nTo, nCap);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_ring_distance_u(size_t nFrom, size_t nTo, size_t nCap)
    __attribute__((alias("gj_ring_distance_u")));
