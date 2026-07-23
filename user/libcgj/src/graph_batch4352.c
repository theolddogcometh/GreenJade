/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4352: ring buffer previous-index modular step.
 *
 * Surface (unique symbols):
 *   size_t gj_ring_prev_u(size_t idx, size_t cap);
 *     - Retreat one slot: (idx + cap - 1) % cap. When cap == 0, return 0
 *       (degenerate ring; avoid modulo by zero).
 *   size_t __gj_ring_prev_u  (alias)
 *   __libcgj_batch4352_marker = "libcgj-batch4352"
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

const char __libcgj_batch4352_marker[] = "libcgj-batch4352";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4352_prev(size_t nIdx, size_t nCap)
{
	if (nCap == 0u) {
		return 0u;
	}
	return (nIdx + nCap - 1u) % nCap;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_prev_u - previous ring index before idx under capacity cap.
 *
 * idx: current slot (typically < cap)
 * cap: ring capacity in slots
 *
 * Returns (idx + cap - 1) % cap, or 0 when cap is 0.
 * Self-contained; no parent wires.
 */
size_t
gj_ring_prev_u(size_t nIdx, size_t nCap)
{
	(void)NULL;
	return b4352_prev(nIdx, nCap);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_ring_prev_u(size_t nIdx, size_t nCap)
    __attribute__((alias("gj_ring_prev_u")));
