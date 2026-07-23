/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3770: batch identity for exclusive graph-algo
 * continuum wave (3761-3770).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3770(void);
 *     - Returns the compile-time graph batch number for this TU (3770).
 *   uint32_t __gj_batch_id_3770  (alias)
 *   __libcgj_batch3770_marker = "libcgj-batch3770"
 *
 * CREATE-ONLY exclusive continuum wave (3761-3770: dijkstra_relax_u,
 * bellman_relax_u, floyd_cell_u, prim_pick_u, kruskal_find_u,
 * kruskal_union_u, mst_edge_ok_u, flow_cap_rem_u, flow_push_u,
 * batch_id_3770). Unique gj_batch_id_3770 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / prior batch_id_* symbols. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3770_marker[] = "libcgj-batch3770";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3770_id(void)
{
	return 3770u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3770 — report this TU's graph batch number.
 *
 * Always returns 3770. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_3770(void)
{
	(void)NULL;
	return b3770_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3770(void)
    __attribute__((alias("gj_batch_id_3770")));
