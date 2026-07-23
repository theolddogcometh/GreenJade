/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3980: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3980(void);
 *     - Returns the compile-time graph batch number for this TU (3980).
 *   uint32_t __gj_batch_id_3980  (alias)
 *   __libcgj_batch3980_marker = "libcgj-batch3980"
 *
 * Exclusive continuum CREATE-ONLY (3971-3980): arena_bump_u,
 * arena_align_u, arena_remaining_u, arena_contains_u, pool_push_free_u,
 * pool_pop_free_u, slab_offset_u, slab_capacity_u, buddy_order_u,
 * batch_id_3980. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3980_marker[] = "libcgj-batch3980";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3980_id(void)
{
	return 3980u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3980 - report this TU's graph batch number.
 *
 * Always returns 3980. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_3980(void)
{
	(void)NULL;
	return b3980_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3980(void)
    __attribute__((alias("gj_batch_id_3980")));
