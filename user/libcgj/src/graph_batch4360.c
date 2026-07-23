/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4360: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4360(void);
 *     - Returns the compile-time graph batch number for this TU (4360).
 *   uint32_t __gj_batch_id_4360  (alias)
 *   __libcgj_batch4360_marker = "libcgj-batch4360"
 *
 * Exclusive continuum CREATE-ONLY (4351-4360: ring_next_u, ring_prev_u,
 * ring_add_u, ring_distance_u, ring_used_u, ring_free_u, ring_full_u,
 * ring_empty_u, ring_index_u, batch_id_4360).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4360_marker[] = "libcgj-batch4360";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4360_id(void)
{
	return 4360u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4360 - report this TU's graph batch number.
 *
 * Always returns 4360. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4360(void)
{
	(void)NULL;
	return b4360_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4360(void)
    __attribute__((alias("gj_batch_id_4360")));
